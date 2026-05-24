import * as vscode from "vscode";
import * as fs from "fs";
import * as os from "os";
import * as path from "path";
import { ChildProcessWithoutNullStreams, execFile, spawn } from "child_process";
import {
    Executable,
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
} from "vscode-languageclient/node";

let client: LanguageClient | undefined;
let outputChannel: vscode.OutputChannel | undefined;
let clientContext: vscode.ExtensionContext | undefined;
let launchedServerPath: string | undefined;
let restartTimer: NodeJS.Timeout | undefined;
let serverWatcher: fs.FSWatcher | undefined;
let formatterRegistration: vscode.Disposable | undefined;
let applyingIndentEdit = false;
let suppressEnterIndentUntil = 0;

type DapMessage = vscode.DebugProtocolMessage & {
    seq?: number;
    type?: string;
    command?: string;
    arguments?: { [key: string]: unknown };
    request_seq?: number;
    success?: boolean;
    message?: string;
    body?: { [key: string]: unknown };
};

type DapBreakpoint = {
    line?: number;
    column?: number;
    condition?: string;
    hitCondition?: string;
    logMessage?: string;
};

type DapSource = {
    path?: string;
};

function execFileAsync(
    command: string,
    args: string[],
    options?: { cwd?: string; env?: NodeJS.ProcessEnv }
): Promise<{ stdout: string; stderr: string }> {
    return new Promise((resolve, reject) => {
        execFile(command, args, options ?? {}, (error, stdout, stderr) => {
            if (error) {
                reject(
                    new Error(
                        stderr?.trim() ||
                            stdout?.trim() ||
                            error.message ||
                            String(error)
                    )
                );
                return;
            }

            resolve({ stdout, stderr });
        });
    });
}

function cleanupLaunchedServer() {
    if (!launchedServerPath) {
        return;
    }

    try {
        fs.rmSync(launchedServerPath, { force: true });
    } catch {
        // Ignore locked or already-removed files.
    }

    launchedServerPath = undefined;
}

function newestExistingPath(paths: string[]): string | undefined {
    let newestPath: string | undefined;
    let newestMtime = -Infinity;

    for (const candidate of paths) {
        try {
            const stat = fs.statSync(candidate);
            if (stat.mtimeMs > newestMtime) {
                newestMtime = stat.mtimeMs;
                newestPath = candidate;
            }
        } catch {
            // Ignore missing or inaccessible candidates.
        }
    }

    return newestPath;
}

function findWorkspaceServer(): string | undefined {
    const exeNames =
        process.platform === "win32"
            ? ["nerd-debug.exe", "nerd.exe"]
            : ["nerd-debug", "nerd"];

    for (const folder of vscode.workspace.workspaceFolders ?? []) {
        const candidates = exeNames.map((exeName) =>
            path.join(folder.uri.fsPath, "_bin", exeName)
        );
        const newestCandidate = newestExistingPath(candidates);
        if (newestCandidate) {
            return newestCandidate;
        }
    }

    return undefined;
}

function findUserServer(): string | undefined {
    const home = process.env.HOME || process.env.USERPROFILE;
    if (!home) {
        return undefined;
    }

    const exeName = process.platform === "win32" ? "nerd.exe" : "nerd";
    const candidate = path.join(home, ".local", "bin", exeName);
    return fs.existsSync(candidate) ? candidate : undefined;
}

function serverCopyDir(context: vscode.ExtensionContext): string {
    return path.join(context.globalStorageUri.fsPath, "server");
}

function stageServerExecutable(
    sourcePath: string,
    context: vscode.ExtensionContext
): string {
    const ext = path.extname(sourcePath);
    const baseName = path.basename(sourcePath, ext);
    const uniqueName =
        `${baseName}-${Date.now()}-${process.pid}-${Math.random().toString(36).slice(2, 8)}${ext}`;
    const targetDir = serverCopyDir(context);
    const targetPath = path.join(targetDir, uniqueName);

    fs.mkdirSync(targetDir, { recursive: true });
    fs.copyFileSync(sourcePath, targetPath);

    if (process.platform !== "win32") {
        const stat = fs.statSync(sourcePath);
        fs.chmodSync(targetPath, stat.mode);
    }

    return targetPath;
}

function getServerEnvironment(sourcePath: string | undefined): NodeJS.ProcessEnv {
    const env: NodeJS.ProcessEnv = { ...process.env };
    if (!sourcePath) {
        return env;
    }

    const modsDir = path.join(path.dirname(sourcePath), "mods");
    if (!fs.existsSync(modsDir) || !fs.statSync(modsDir).isDirectory()) {
        return env;
    }

    const separator = process.platform === "win32" ? ";" : ":";
    const existing = env.NERD_LIB_PATH?.trim();
    env.NERD_LIB_PATH = existing ? `${modsDir}${separator}${existing}` : modsDir;
    return env;
}

function getServerExecutable(
    context: vscode.ExtensionContext
): { executable: Executable; sourcePath?: string } {
    const config = vscode.workspace.getConfiguration("nerd");
    const configuredPath = config.get<string>("languageServer.path", "").trim();
    const args = config.get<string[]>("languageServer.args", ["lsp"]);
    const sourcePath =
        configuredPath || findWorkspaceServer() || findUserServer();
    const command = sourcePath
        ? stageServerExecutable(sourcePath, context)
        : "nerd";
    const env = getServerEnvironment(sourcePath);

    return {
        executable: { command, args, options: { env } },
        sourcePath,
    };
}

function disposeServerWatcher() {
    serverWatcher?.close();
    serverWatcher = undefined;
}

function getToolExecutablePath(): string {
    const config = vscode.workspace.getConfiguration("nerd");
    const configuredPath = config.get<string>("languageServer.path", "").trim();
    return configuredPath || findWorkspaceServer() || findUserServer() || "nerd";
}

function workspaceFolderForDocument(
    document: vscode.TextDocument
): vscode.WorkspaceFolder | undefined {
    return vscode.workspace.getWorkspaceFolder(document.uri);
}

function executableSuffix(): string {
    return process.platform === "win32" ? ".exe" : "";
}

function codeLldbAdapterPath(): string | undefined {
    const extension = vscode.extensions.getExtension("vadimcn.vscode-lldb");
    if (!extension) {
        return undefined;
    }

    const executableName = process.platform === "win32" ? "codelldb.exe" : "codelldb";
    const candidate = path.join(extension.extensionPath, "adapter", executableName);
    return fs.existsSync(candidate) ? candidate : undefined;
}

function encodeDapMessage(message: DapMessage): Buffer {
    const payload = Buffer.from(JSON.stringify(message), "utf8");
    return Buffer.concat([
        Buffer.from(`Content-Length: ${payload.length}\r\n\r\n`, "ascii"),
        payload,
    ]);
}

function isNerdSourcePath(sourcePath: string | undefined): sourcePath is string {
    return sourcePath !== undefined && path.extname(sourcePath) === ".n";
}

function stripNerdLineComment(line: string): string {
    const comment = line.indexOf("--");
    return comment >= 0 ? line.slice(0, comment) : line;
}

function isNerdExecutableBreakpointLine(line: string): boolean {
    const text = stripNerdLineComment(line).trim();
    if (!text) {
        return false;
    }
    if (text === "{" || text === "}") {
        return false;
    }
    if (text.endsWith("{")) {
        return false;
    }
    if (/^[A-Za-z_][A-Za-z0-9_.$]*\s*::/.test(text)) {
        return false;
    }
    return true;
}

function nearestNerdBreakpointLine(lines: string[], requestedLine: number): number {
    const start = Math.max(1, Math.min(requestedLine, lines.length));
    for (let line = start; line <= lines.length; ++line) {
        if (isNerdExecutableBreakpointLine(lines[line - 1])) {
            return line;
        }
    }
    for (let line = start - 1; line >= 1; --line) {
        if (isNerdExecutableBreakpointLine(lines[line - 1])) {
            return line;
        }
    }
    return requestedLine;
}

function normalizeNerdBreakpointRequest(message: DapMessage): DapMessage {
    if (message.type !== "request" || message.command !== "setBreakpoints") {
        return message;
    }

    const args = message.arguments;
    const source = args?.source as DapSource | undefined;
    if (!args || !isNerdSourcePath(source?.path)) {
        return message;
    }

    let lines: string[];
    try {
        lines = fs.readFileSync(source.path, "utf8").split(/\r?\n/);
    } catch {
        return message;
    }

    const breakpoints = args.breakpoints as DapBreakpoint[] | undefined;
    if (!Array.isArray(breakpoints)) {
        return message;
    }

    let movedCount = 0;
    const normalizedBreakpoints = breakpoints.map((breakpoint) => {
        if (breakpoint.line === undefined) {
            return breakpoint;
        }
        const normalizedLine = nearestNerdBreakpointLine(lines, breakpoint.line);
        if (normalizedLine === breakpoint.line) {
            return breakpoint;
        }
        movedCount += 1;
        return { ...breakpoint, line: normalizedLine };
    });
    if (movedCount > 0) {
        outputChannel?.appendLine(
            `Moved ${movedCount} Nerd breakpoint(s) in ${path.basename(source.path)} to executable lines.`
        );
    }

    return {
        ...message,
        arguments: {
            ...args,
            breakpoints: normalizedBreakpoints,
        },
    };
}

function dynamicArrayHeaderWatchExpression(expression: string): string | undefined {
    const match = expression
        .trim()
        .match(/^([A-Za-z_][A-Za-z0-9_.$]*)\s*\.\s*(count|capacity|data)$/);
    if (!match) {
        return undefined;
    }

    const base = match[1];
    const field = match[2];
    if (field === "data") {
        return base;
    }

    const offset = field === "count" ? 16 : 8;
    return `${base} ? *((unsigned long long *)((char *)${base} - ${offset})) : 0`;
}

function retryableDynamicArrayWatch(message: DapMessage): string | undefined {
    if (
        message.type !== "request" ||
        message.command !== "evaluate" ||
        typeof message.arguments?.expression !== "string"
    ) {
        return undefined;
    }
    return dynamicArrayHeaderWatchExpression(message.arguments.expression);
}

class DapMessageReader {
    private buffer = Buffer.alloc(0);

    constructor(private readonly onMessage: (message: DapMessage) => void) {}

    accept(chunk: Buffer): void {
        this.buffer = Buffer.concat([this.buffer, chunk]);

        while (true) {
            const headerEnd = this.buffer.indexOf("\r\n\r\n");
            if (headerEnd < 0) {
                return;
            }

            const header = this.buffer.subarray(0, headerEnd).toString("ascii");
            const lengthLine = header
                .split("\r\n")
                .find((line) => line.toLowerCase().startsWith("content-length:"));
            if (!lengthLine) {
                throw new Error("Debug adapter message is missing Content-Length");
            }

            const payloadLength = Number.parseInt(lengthLine.split(":", 2)[1].trim(), 10);
            const payloadStart = headerEnd + 4;
            const payloadEnd = payloadStart + payloadLength;
            if (this.buffer.length < payloadEnd) {
                return;
            }

            const payload = this.buffer.subarray(payloadStart, payloadEnd).toString("utf8");
            this.buffer = this.buffer.subarray(payloadEnd);
            this.onMessage(JSON.parse(payload) as DapMessage);
        }
    }
}

class NerdCodeLldbDebugAdapter implements vscode.DebugAdapter {
    private readonly emitter = new vscode.EventEmitter<vscode.DebugProtocolMessage>();
    readonly onDidSendMessage = this.emitter.event;
    private readonly process: ChildProcessWithoutNullStreams;
    private readonly reader: DapMessageReader;
    private nextInternalSeq = 1_000_000_000;
    private readonly pendingDynamicArrayRetries = new Map<number, DapMessage>();
    private readonly dynamicArrayRetryResponses = new Map<number, number>();

    constructor(adapterPath: string) {
        this.process = spawn(adapterPath, [], {
            env: process.env,
            stdio: ["pipe", "pipe", "pipe"],
        });
        this.reader = new DapMessageReader((message) => this.forwardFromCodeLldb(message));

        this.process.stdout.on("data", (chunk: Buffer) => {
            try {
                this.reader.accept(chunk);
            } catch (error) {
                this.sendError(`CodeLLDB protocol error: ${String(error)}`);
            }
        });
        this.process.stderr.on("data", (chunk: Buffer) => {
            outputChannel?.append(chunk.toString("utf8"));
        });
        this.process.on("error", (error) => {
            this.sendError(`Could not start CodeLLDB: ${error.message}`);
        });
    }

    handleMessage(message: vscode.DebugProtocolMessage): void {
        const dapMessage = message as DapMessage;
        const retryExpression = retryableDynamicArrayWatch(dapMessage);
        if (retryExpression && dapMessage.seq !== undefined) {
            this.pendingDynamicArrayRetries.set(dapMessage.seq, {
                ...dapMessage,
                arguments: {
                    ...dapMessage.arguments,
                    expression: retryExpression,
                },
            });
        }

        if (!this.process.stdin.destroyed) {
            this.process.stdin.write(
                encodeDapMessage(normalizeNerdBreakpointRequest(dapMessage))
            );
        }
    }

    dispose(): void {
        this.process.kill();
        this.emitter.dispose();
    }

    private forwardFromCodeLldb(message: DapMessage): void {
        if (message.type === "response" && message.command === "initialize" && message.body) {
            message.body.supportsDelayedStackTraceLoading = false;
        }

        if (message.type === "response" && message.request_seq !== undefined) {
            const originalRequest = this.dynamicArrayRetryResponses.get(
                message.request_seq
            );
            if (originalRequest !== undefined) {
                this.dynamicArrayRetryResponses.delete(message.request_seq);
                message.request_seq = originalRequest;
                this.emitter.fire(message);
                return;
            }

            const retry = this.pendingDynamicArrayRetries.get(message.request_seq);
            this.pendingDynamicArrayRetries.delete(message.request_seq);
            if (
                retry &&
                message.command === "evaluate" &&
                message.success === false &&
                !this.process.stdin.destroyed
            ) {
                const retrySeq = this.nextInternalSeq++;
                retry.seq = retrySeq;
                this.dynamicArrayRetryResponses.set(retrySeq, message.request_seq);
                this.process.stdin.write(encodeDapMessage(retry));
                return;
            }
        }
        this.emitter.fire(message);
    }

    private sendError(message: string): void {
        outputChannel?.appendLine(message);
        outputChannel?.show(true);
    }
}

class NerdDebugAdapterFactory implements vscode.DebugAdapterDescriptorFactory {
    createDebugAdapterDescriptor(): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        const adapterPath = codeLldbAdapterPath();
        if (!adapterPath) {
            void vscode.window.showErrorMessage(
                "Nerd debugging requires the CodeLLDB extension."
            );
            return undefined;
        }

        return new vscode.DebugAdapterInlineImplementation(
            new NerdCodeLldbDebugAdapter(adapterPath)
        );
    }
}

function nerdDebugOutputPath(document: vscode.TextDocument): string {
    const workspaceFolder = workspaceFolderForDocument(document);
    const baseDir = workspaceFolder?.uri.fsPath ?? path.dirname(document.fileName);
    const outputDir = path.join(baseDir, "_tmp", "debug");
    const parsed = path.parse(document.fileName);
    return path.join(outputDir, `${parsed.name}${executableSuffix()}`);
}

async function buildActiveNerdDocumentForDebug(): Promise<string | undefined> {
    const editor = vscode.window.activeTextEditor;
    const document = editor?.document;

    if (!document || document.languageId !== "nerd" || document.uri.scheme !== "file") {
        void vscode.window.showErrorMessage("Open a Nerd source file before debugging.");
        return undefined;
    }

    if (document.isDirty) {
        await document.save();
    }

    const executablePath = getToolExecutablePath();
    const outputPath = nerdDebugOutputPath(document);
    const outputDir = path.dirname(outputPath);
    const sourcePath = document.uri.fsPath;
    const workspaceFolder = workspaceFolderForDocument(document);
    const cwd = workspaceFolder?.uri.fsPath ?? path.dirname(sourcePath);
    const toolSourcePath =
        executablePath !== "nerd"
            ? executablePath
            : findWorkspaceServer() || findUserServer();
    const env = getServerEnvironment(toolSourcePath);

    fs.mkdirSync(outputDir, { recursive: true });
    outputChannel?.appendLine(`Building debug executable: ${sourcePath}`);
    outputChannel?.appendLine(`Output: ${outputPath}`);

    try {
        const result = await execFileAsync(
            executablePath,
            ["build", "--output", outputPath, sourcePath],
            { cwd, env }
        );
        if (result.stdout.trim()) {
            outputChannel?.appendLine(result.stdout.trimEnd());
        }
        if (result.stderr.trim()) {
            outputChannel?.appendLine(result.stderr.trimEnd());
        }
        return outputPath;
    } catch (error) {
        outputChannel?.appendLine(String(error));
        outputChannel?.show(true);
        void vscode.window.showErrorMessage(`Nerd debug build failed: ${String(error)}`);
        return undefined;
    }
}

async function debugActiveNerdFileWithCodeLLDB() {
    const editor = vscode.window.activeTextEditor;
    const document = editor?.document;
    const executablePath = await buildActiveNerdDocumentForDebug();
    if (!document || !executablePath) {
        return;
    }

    const workspaceFolder = workspaceFolderForDocument(document);
    const cwd = workspaceFolder?.uri.fsPath ?? path.dirname(document.uri.fsPath);
    const started = await vscode.debug.startDebugging(workspaceFolder, {
        type: "nerd",
        request: "launch",
        name: `Debug ${path.basename(document.uri.fsPath)}`,
        program: executablePath,
        args: [],
        cwd,
    });

    if (!started) {
        outputChannel?.show(true);
        void vscode.window.showErrorMessage(
            "Could not start CodeLLDB. Install the CodeLLDB extension and try again."
        );
    }
}

async function provideFormattedText(
    document: vscode.TextDocument
): Promise<string> {
    const executablePath = getToolExecutablePath();
    const tempDir = fs.mkdtempSync(path.join(os.tmpdir(), "nerd-format-"));
    const inputPath = path.join(tempDir, `input${path.extname(document.fileName) || ".n"}`);
    const outputPath = path.join(tempDir, "output.n");

    try {
        fs.writeFileSync(inputPath, document.getText(), "utf8");
        await execFileAsync(executablePath, ["format", inputPath, "-o", outputPath]);
        return fs.readFileSync(outputPath, "utf8");
    } finally {
        try {
            fs.rmSync(tempDir, { recursive: true, force: true });
        } catch {
            // Ignore cleanup failures.
        }
    }
}

function fullDocumentRange(document: vscode.TextDocument): vscode.Range {
    return new vscode.Range(
        document.positionAt(0),
        document.positionAt(document.getText().length)
    );
}

function registerFormatter(context: vscode.ExtensionContext) {
    formatterRegistration?.dispose();
    formatterRegistration = vscode.languages.registerDocumentFormattingEditProvider(
        { scheme: "file", language: "nerd" },
        {
            async provideDocumentFormattingEdits(document) {
                const formattedText = await provideFormattedText(document);
                suppressEnterIndentUntil = Date.now() + 1500;
                return [
                    vscode.TextEdit.replace(
                        fullDocumentRange(document),
                        formattedText
                    ),
                ];
            },
        }
    );
    context.subscriptions.push(formatterRegistration);
}

function stripLineComment(line: string): string {
    let inString = false;
    let escaped = false;

    for (let i = 0; i < line.length; i++) {
        const ch = line[i];

        if (inString) {
            if (escaped) {
                escaped = false;
            } else if (ch === "\\") {
                escaped = true;
            } else if (ch === "\"") {
                inString = false;
            }
            continue;
        }

        if (ch === "\"") {
            inString = true;
            continue;
        }

        if (ch === "-" && line[i + 1] === "-") {
            return line.slice(0, i);
        }
    }

    return line;
}

function trimCode(line: string): string {
    return stripLineComment(line).replace(/\s+$/, "");
}

function unclosedOpenDelimiter(line: string): { ch: string; column: number } | undefined {
    const code = stripLineComment(line);
    const stack: { ch: string; column: number }[] = [];
    let inString = false;
    let escaped = false;

    for (let i = 0; i < code.length; i++) {
        const ch = code[i];

        if (inString) {
            if (escaped) {
                escaped = false;
            } else if (ch === "\\") {
                escaped = true;
            } else if (ch === "\"") {
                inString = false;
            }
            continue;
        }

        if (ch === "\"") {
            inString = true;
        } else if (ch === "{" || ch === "[" || ch === "(") {
            stack.push({ ch, column: i });
        } else if ((ch === "}" || ch === "]" || ch === ")") && stack.length > 0) {
            stack.pop();
        }
    }

    return stack.at(-1);
}

function matchingOpenContinuationLine(lines: string[], lineIndex: number): number | undefined {
    let depth = 0;

    for (let current = lineIndex; current >= 0; current--) {
        const code = stripLineComment(lines[current]);
        for (let i = code.length - 1; i >= 0; i--) {
            const ch = code[i];
            if (ch === ")" || ch === "]") {
                depth++;
            } else if (ch === "(" || ch === "[") {
                depth--;
                if (depth === 0) {
                    return current;
                }
            }
        }
    }

    return undefined;
}

function leadingWhitespaceWidth(line: string): number {
    const match = line.match(/^\s*/);
    return match ? match[0].length : 0;
}

function previousNonBlankLine(lines: string[], lineIndex: number): number | undefined {
    for (let current = lineIndex - 1; current >= 0; current--) {
        if (lines[current].trim().length > 0) {
            return current;
        }
    }
    return undefined;
}

function computeNerdIndent(lines: string[], lineIndex: number, shiftWidth = 4): number {
    const prevIndex = previousNonBlankLine(lines, lineIndex);
    if (prevIndex === undefined) {
        return 0;
    }

    let indent = leadingWhitespaceWidth(lines[prevIndex]);
    const prev = trimCode(lines[prevIndex]);
    const line = trimCode(lines[lineIndex] ?? "");

    if (/[)\]]/.test(prev)) {
        const openLine = matchingOpenContinuationLine(lines, prevIndex);
        if (openLine !== undefined) {
            indent = leadingWhitespaceWidth(lines[openLine]);
        }
    }

    const open = unclosedOpenDelimiter(prev);
    if (open?.ch === "{") {
        indent += shiftWidth;
    } else if (open?.ch === "(" || open?.ch === "[") {
        indent = open.column + 1;
    }

    if (/^\s*[}\])]/.test(line)) {
        indent -= shiftWidth;
    }

    return Math.max(indent, 0);
}

async function applyNerdEnterIndent(document: vscode.TextDocument, line: number) {
    if (line < 0 || line >= document.lineCount) {
        return;
    }

    const lines = Array.from({ length: document.lineCount }, (_, i) =>
        document.lineAt(i).text
    );
    const expectedIndent = computeNerdIndent(lines, line);
    const text = document.lineAt(line).text;
    const actualIndentText = text.match(/^\s*/)?.[0] ?? "";
    const expectedIndentText = " ".repeat(expectedIndent);

    if (actualIndentText === expectedIndentText) {
        return;
    }

    applyingIndentEdit = true;
    try {
        const edit = new vscode.WorkspaceEdit();
        edit.replace(
            document.uri,
            new vscode.Range(line, 0, line, actualIndentText.length),
            expectedIndentText
        );
        await vscode.workspace.applyEdit(edit);
    } finally {
        applyingIndentEdit = false;
    }
}

function registerEnterIndentation(context: vscode.ExtensionContext) {
    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument((event) => {
            if (
                applyingIndentEdit ||
                event.document.languageId !== "nerd" ||
                Date.now() < suppressEnterIndentUntil
            ) {
                return;
            }

            for (const change of event.contentChanges) {
                if (!/^\r?\n[ \t]*$/.test(change.text)) {
                    continue;
                }

                const insertedLine = change.range.start.line + 1;
                void applyNerdEnterIndent(event.document, insertedLine);
            }
        })
    );
}

function scheduleRestart(reason: string) {
    if (!clientContext) {
        return;
    }

    outputChannel?.appendLine(`Scheduling server restart: ${reason}`);

    if (restartTimer) {
        clearTimeout(restartTimer);
    }

    restartTimer = setTimeout(() => {
        restartTimer = undefined;
        void restartLanguageServer(reason);
    }, 250);
}

function watchServerSource(sourcePath: string | undefined) {
    disposeServerWatcher();

    if (!sourcePath) {
        return;
    }

    try {
        const watchDir = path.dirname(sourcePath);
        const watchName = path.basename(sourcePath);
        serverWatcher = fs.watch(watchDir, (_eventType, filename) => {
            if (!filename || filename.toString() === watchName) {
                scheduleRestart(`detected change in ${sourcePath}`);
            }
        });
    } catch (error) {
        outputChannel?.appendLine(
            `Failed to watch server executable ${sourcePath}: ${String(error)}`
        );
    }
}

async function stopLanguageServer() {
    disposeServerWatcher();

    if (restartTimer) {
        clearTimeout(restartTimer);
        restartTimer = undefined;
    }

    if (client) {
        const currentClient = client;
        client = undefined;
        await currentClient.stop();
    }

    cleanupLaunchedServer();
}

async function startLanguageServer(context: vscode.ExtensionContext) {
    const { executable: serverExecutable, sourcePath } = getServerExecutable(
        context
    );

    launchedServerPath =
        sourcePath && serverExecutable.command !== "nerd"
            ? serverExecutable.command
            : undefined;

    outputChannel?.appendLine(
        `Starting server: ${serverExecutable.command} ${serverExecutable.args?.join(" ") ?? ""}`.trim()
    );

    const serverOptions: ServerOptions = {
        run: serverExecutable,
        debug: serverExecutable,
    };

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{ scheme: "file", language: "nerd" }],
        outputChannel,
        middleware: {
            async provideCompletionItem(document, position, context, token, next) {
                const result = await next(document, position, context, token);
                if (!result) {
                    return result;
                }
                if (Array.isArray(result)) {
                    return new vscode.CompletionList(result, true);
                }
                result.isIncomplete = true;
                return result;
            },
        },
    };

    const nextClient = new LanguageClient(
        "nerdLanguageServer",
        "Nerd Language Server",
        serverOptions,
        clientOptions
    );

    client = nextClient;
    watchServerSource(sourcePath);
    await nextClient.start();
}

async function restartLanguageServer(reason: string) {
    if (!clientContext) {
        return;
    }

    outputChannel?.appendLine(`Restarting server: ${reason}`);
    await stopLanguageServer();
    await startLanguageServer(clientContext);
}

export function activate(
    context: vscode.ExtensionContext
): Thenable<void> | undefined {
    clientContext = context;
    outputChannel = vscode.window.createOutputChannel("Nerd Language Server");
    context.subscriptions.push(outputChannel);
    context.subscriptions.push({
        dispose: () => {
            disposeServerWatcher();
            cleanupLaunchedServer();
            formatterRegistration?.dispose();
            formatterRegistration = undefined;
        },
    });

    fs.mkdirSync(context.globalStorageUri.fsPath, { recursive: true });
    try {
        fs.rmSync(serverCopyDir(context), { recursive: true, force: true });
    } catch {
        // Ignore stale or locked files from previous sessions.
    }

    registerFormatter(context);
    registerEnterIndentation(context);
    context.subscriptions.push(
        vscode.debug.registerDebugAdapterDescriptorFactory(
            "nerd",
            new NerdDebugAdapterFactory()
        )
    );
    context.subscriptions.push(
        vscode.commands.registerCommand(
            "nerd.restartLanguageServer",
            async () => {
                await restartLanguageServer("manual restart command");
            }
        )
    );
    context.subscriptions.push(
        vscode.commands.registerCommand(
            "nerd.buildActiveFileForDebug",
            buildActiveNerdDocumentForDebug
        )
    );
    context.subscriptions.push(
        vscode.commands.registerCommand(
            "nerd.debugActiveFileWithCodeLLDB",
            debugActiveNerdFileWithCodeLLDB
        )
    );
    return startLanguageServer(context);
}

export function deactivate(): Thenable<void> | undefined {
    outputChannel?.appendLine("Stopping server");
    return stopLanguageServer();
}
