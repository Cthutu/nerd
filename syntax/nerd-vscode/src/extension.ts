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

type DapScope = {
    name?: string;
    variablesReference?: number;
    expensive?: boolean;
};

type DapVariable = {
    evaluateName?: string;
    name?: string;
    presentationHint?: { [key: string]: unknown };
    result?: string;
    type?: string;
    value?: string;
    variablesReference?: number;
};

type PendingVariablesRequest = {
    variablesReference: number;
};

type PendingEvaluateRequest = {
    context?: string;
};

type SyntheticDynamicArray = {
    baseExpression: string;
    frameId: number;
    itemType: string;
};

type PendingSyntheticKind = "count" | "capacity" | "data" | "item";

type PendingSyntheticEvaluation = {
    requestSeq: number;
    kind: PendingSyntheticKind;
    itemIndex?: number;
};

type PendingSyntheticExpansion = {
    originalRequest: DapMessage;
    array: SyntheticDynamicArray;
    count?: number;
    capacity?: string;
    data?: string;
    itemResults: Map<number, DapVariable>;
    pending: number;
};

const nerdTupleSummaryPython = [
    "def nerd_tuple_summary(valobj, internal_dict):",
    "    parts = []",
    "    for index in range(valobj.GetNumChildren()):",
    "        child = valobj.GetChildAtIndex(index)",
    '        parts.append(child.GetSummary() or child.GetValue() or "<unavailable>")',
    '    return "(" + ", ".join(parts) + ")"',
].join("\n");

const nerdLldbInitCommands = [
    `script exec(${JSON.stringify(nerdTupleSummaryPython)})`,
    `type summary add string --summary-string "\${var.data}"`,
    `type summary add -x "^\\\\(.+\\\\)$" -F nerd_tuple_summary`,
];

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

function collectDynamicArrayDeclarationsFromText(text: string, names: Set<string>) {
    const declaration =
        /(?:^|[\s{])([A-Za-z_][A-Za-z0-9_]*)\s*:?\s*\[\s*\.\.\s*\]/g;
    let match: RegExpExecArray | null;
    while ((match = declaration.exec(text)) !== null) {
        names.add(match[1]);
    }
}

function collectDynamicArrayDeclarationsFromFile(filePath: string, names: Set<string>) {
    try {
        collectDynamicArrayDeclarationsFromText(fs.readFileSync(filePath, "utf8"), names);
    } catch {
        // Ignore files that disappear or cannot be read while the debug session starts.
    }
}

function collectDynamicArrayDeclarationsFromDir(dirPath: string, names: Set<string>) {
    let entries: fs.Dirent[];
    try {
        entries = fs.readdirSync(dirPath, { withFileTypes: true });
    } catch {
        return;
    }

    for (const entry of entries) {
        if (entry.name === ".git" || entry.name === "node_modules" || entry.name === "_obj") {
            continue;
        }
        const entryPath = path.join(dirPath, entry.name);
        if (entry.isDirectory()) {
            collectDynamicArrayDeclarationsFromDir(entryPath, names);
        } else if (entry.isFile() && path.extname(entry.name) === ".n") {
            collectDynamicArrayDeclarationsFromFile(entryPath, names);
        }
    }
}

function collectDynamicArrayDeclarationsNearPath(startPath: string, names: Set<string>) {
    let current = fs.existsSync(startPath) && fs.statSync(startPath).isDirectory()
        ? startPath
        : path.dirname(startPath);
    for (;;) {
        collectDynamicArrayDeclarationsFromDir(path.join(current, "mods"), names);
        collectDynamicArrayDeclarationsFromDir(path.join(current, "_bin", "mods"), names);

        const parent = path.dirname(current);
        if (parent === current) {
            break;
        }
        current = parent;
    }
}

function parseUnsignedResult(value: string | undefined): number | undefined {
    if (!value) {
        return undefined;
    }
    const match = value.trim().match(/^(?:0x[0-9a-fA-F]+|\d+)/);
    if (!match) {
        return undefined;
    }
    const parsed = Number.parseInt(match[0], match[0].startsWith("0x") ? 16 : 10);
    return Number.isFinite(parsed) && parsed >= 0 ? parsed : undefined;
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
    private readonly pendingScopeRequests = new Map<number, number>();
    private readonly scopeFrames = new Map<number, number>();
    private readonly pendingVariablesRequests = new Map<number, PendingVariablesRequest>();
    private readonly pendingEvaluateRequests = new Map<number, PendingEvaluateRequest>();
    private readonly dynamicArrayDeclarations = new Set<string>();
    private readonly syntheticDynamicArrays = new Map<number, SyntheticDynamicArray>();
    private readonly pendingSyntheticEvaluations = new Map<number, PendingSyntheticEvaluation>();
    private readonly pendingSyntheticExpansions = new Map<number, PendingSyntheticExpansion>();

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
        this.observeClientRequest(dapMessage);
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

        if (this.handleSyntheticVariablesRequest(dapMessage)) {
            return;
        }

        if (!this.process.stdin.destroyed) {
            this.process.stdin.write(
                encodeDapMessage(
                    this.normalizeLaunchRequest(normalizeNerdBreakpointRequest(dapMessage))
                )
            );
        }
    }

    dispose(): void {
        this.process.kill();
        this.emitter.dispose();
    }

    private forwardFromCodeLldb(message: DapMessage): void {
        if (this.handleSyntheticEvaluationResponse(message)) {
            return;
        }

        if (message.type === "response" && message.command === "initialize" && message.body) {
            message.body.supportsDelayedStackTraceLoading = false;
        }

        if (message.type === "response" && message.command === "scopes" && message.body) {
            const scopes = message.body.scopes as DapScope[] | undefined;
            const frameId =
                message.request_seq !== undefined
                    ? this.pendingScopeRequests.get(message.request_seq)
                    : undefined;
            if (message.request_seq !== undefined) {
                this.pendingScopeRequests.delete(message.request_seq);
            }
            if (Array.isArray(scopes)) {
                for (const scope of scopes) {
                    if (scope.name === "Global") {
                        scope.name = "Nerd Globals";
                    }
                    if (
                        frameId !== undefined &&
                        typeof scope.variablesReference === "number"
                    ) {
                        this.scopeFrames.set(scope.variablesReference, frameId);
                    }
                }
            }
        }

        if (message.type === "response" && message.command === "variables") {
            this.rewriteVariablesResponse(message);
        }

        if (message.type === "response" && message.command === "evaluate") {
            this.rewriteEvaluateResponse(message);
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

    private observeClientRequest(message: DapMessage) {
        if (message.type !== "request" || message.seq === undefined) {
            return;
        }

        if (message.command === "launch") {
            this.collectDynamicArrayDeclarations(message);
        } else if (message.command === "setBreakpoints") {
            const source = message.arguments?.source as DapSource | undefined;
            if (source?.path) {
                collectDynamicArrayDeclarationsFromFile(
                    source.path,
                    this.dynamicArrayDeclarations
                );
                collectDynamicArrayDeclarationsNearPath(
                    source.path,
                    this.dynamicArrayDeclarations
                );
            }
        } else if (
            message.command === "scopes" &&
            typeof message.arguments?.frameId === "number"
        ) {
            this.pendingScopeRequests.set(message.seq, message.arguments.frameId);
        } else if (
            message.command === "variables" &&
            typeof message.arguments?.variablesReference === "number"
        ) {
            this.pendingVariablesRequests.set(message.seq, {
                variablesReference: message.arguments.variablesReference,
            });
        } else if (message.command === "evaluate") {
            const context = message.arguments?.context;
            this.pendingEvaluateRequests.set(message.seq, {
                context: typeof context === "string" ? context : undefined,
            });
        }
    }

    private normalizeLaunchRequest(message: DapMessage): DapMessage {
        if (message.type !== "request" || message.command !== "launch") {
            return message;
        }
        const userInitCommands = message.arguments?.initCommands;
        const initCommands = Array.isArray(userInitCommands)
            ? [...nerdLldbInitCommands, ...userInitCommands]
            : nerdLldbInitCommands;
        return {
            ...message,
            arguments: {
                ...message.arguments,
                expressions: message.arguments?.expressions ?? "native",
                initCommands,
            },
        };
    }

    private collectDynamicArrayDeclarations(message: DapMessage) {
        this.dynamicArrayDeclarations.clear();
        const args = message.arguments ?? {};
        if (typeof args.cwd === "string") {
            collectDynamicArrayDeclarationsFromDir(args.cwd, this.dynamicArrayDeclarations);
            collectDynamicArrayDeclarationsNearPath(args.cwd, this.dynamicArrayDeclarations);
        }
        if (typeof args.program === "string") {
            collectDynamicArrayDeclarationsNearPath(args.program, this.dynamicArrayDeclarations);
        }
        for (const folder of vscode.workspace.workspaceFolders ?? []) {
            collectDynamicArrayDeclarationsFromDir(
                folder.uri.fsPath,
                this.dynamicArrayDeclarations
            );
            collectDynamicArrayDeclarationsNearPath(
                folder.uri.fsPath,
                this.dynamicArrayDeclarations
            );
        }
        const env = process.env.NERD_LIB_PATH?.split(process.platform === "win32" ? ";" : ":");
        for (const dir of env ?? []) {
            if (dir.trim()) {
                collectDynamicArrayDeclarationsFromDir(dir.trim(), this.dynamicArrayDeclarations);
            }
        }
    }

    private rewriteVariablesResponse(message: DapMessage) {
        if (message.request_seq === undefined) {
            return;
        }
        const request = this.pendingVariablesRequests.get(message.request_seq);
        this.pendingVariablesRequests.delete(message.request_seq);
        if (!request || !message.body) {
            return;
        }
        const frameId = this.scopeFrames.get(request.variablesReference);
        if (frameId === undefined) {
            return;
        }

        const variables = message.body.variables as DapVariable[] | undefined;
        if (!Array.isArray(variables)) {
            return;
        }

        for (const variable of variables) {
            const name = variable.name;
            const evaluateName = variable.evaluateName ?? name;
            if (
                !name ||
                !evaluateName ||
                !this.dynamicArrayDeclarations.has(name) ||
                !variable.type?.endsWith("*")
            ) {
                continue;
            }

            const reference = this.nextInternalSeq++;
            const itemType = variable.type.replace(/\s*\*$/, "").trim();
            this.syntheticDynamicArrays.set(reference, {
                baseExpression: evaluateName,
                frameId,
                itemType,
            });
            variable.value = `${variable.type} dynamic array`;
            variable.variablesReference = reference;
        }
    }

    private rewriteEvaluateResponse(message: DapMessage) {
        if (message.request_seq === undefined || !message.body) {
            return;
        }
        const request = this.pendingEvaluateRequests.get(message.request_seq);
        this.pendingEvaluateRequests.delete(message.request_seq);
        if (request?.context !== "hover" || message.success === false) {
            return;
        }

        const type = message.body.type;
        const result = message.body.result;
        if (typeof type !== "string" || typeof result !== "string") {
            return;
        }
        if (result.startsWith(`(${type}) `)) {
            return;
        }
        message.body.result = `(${type}) ${result}`;
    }

    private handleSyntheticVariablesRequest(message: DapMessage): boolean {
        if (
            message.type !== "request" ||
            message.command !== "variables" ||
            message.seq === undefined ||
            typeof message.arguments?.variablesReference !== "number"
        ) {
            return false;
        }

        const array = this.syntheticDynamicArrays.get(message.arguments.variablesReference);
        if (!array) {
            return false;
        }

        this.pendingVariablesRequests.delete(message.seq);
        this.startSyntheticDynamicArrayExpansion(message, array);
        return true;
    }

    private startSyntheticDynamicArrayExpansion(
        originalRequest: DapMessage,
        array: SyntheticDynamicArray
    ) {
        const expansion: PendingSyntheticExpansion = {
            originalRequest,
            array,
            itemResults: new Map(),
            pending: 0,
        };
        this.pendingSyntheticExpansions.set(originalRequest.seq ?? 0, expansion);
        this.requestSyntheticEvaluation(expansion, "count", this.dynamicArrayHeaderExpression(array, 2));
        this.requestSyntheticEvaluation(
            expansion,
            "capacity",
            this.dynamicArrayHeaderExpression(array, 1)
        );
        this.requestSyntheticEvaluation(expansion, "data", `(void*)${array.baseExpression}`);
    }

    private dynamicArrayHeaderExpression(array: SyntheticDynamicArray, offset: number): string {
        return `((unsigned long long *)${array.baseExpression})[-${offset}]`;
    }

    private requestSyntheticEvaluation(
        expansion: PendingSyntheticExpansion,
        kind: PendingSyntheticKind,
        expression: string,
        itemIndex?: number
    ) {
        if (this.process.stdin.destroyed) {
            return;
        }
        const seq = this.nextInternalSeq++;
        expansion.pending += 1;
        this.pendingSyntheticEvaluations.set(seq, {
            requestSeq: expansion.originalRequest.seq ?? 0,
            kind,
            itemIndex,
        });
        this.process.stdin.write(
            encodeDapMessage({
                seq,
                type: "request",
                command: "evaluate",
                arguments: {
                    expression,
                    frameId: expansion.array.frameId,
                    context: "watch",
                },
            })
        );
    }

    private handleSyntheticEvaluationResponse(message: DapMessage): boolean {
        if (message.type !== "response" || message.request_seq === undefined) {
            return false;
        }
        const pending = this.pendingSyntheticEvaluations.get(message.request_seq);
        if (!pending) {
            return false;
        }
        this.pendingSyntheticEvaluations.delete(message.request_seq);

        const expansion = this.pendingSyntheticExpansions.get(pending.requestSeq);
        if (!expansion) {
            return true;
        }

        expansion.pending -= 1;
        const body = message.body as DapVariable | undefined;
        if (message.success !== false && body) {
            if (pending.kind === "count") {
                expansion.count = parseUnsignedResult(body.result as string | undefined);
            } else if (pending.kind === "capacity") {
                expansion.capacity = body.result as string | undefined;
            } else if (pending.kind === "data") {
                expansion.data = body.result as string | undefined;
            } else if (pending.kind === "item" && pending.itemIndex !== undefined) {
                expansion.itemResults.set(pending.itemIndex, {
                    evaluateName: `${expansion.array.baseExpression}[${pending.itemIndex}]`,
                    name: `[${pending.itemIndex}]`,
                    type: body.type,
                    value: body.result,
                    variablesReference: body.variablesReference ?? 0,
                });
            }
        } else if (pending.kind === "count") {
            expansion.count = 0;
        }

        if (pending.kind === "count" && expansion.count !== undefined) {
            const itemLimit = Math.min(expansion.count, 100);
            for (let i = 0; i < itemLimit; ++i) {
                this.requestSyntheticEvaluation(
                    expansion,
                    "item",
                    `${expansion.array.baseExpression}[${i}]`,
                    i
                );
            }
        }

        this.maybeFinishSyntheticExpansion(expansion);
        return true;
    }

    private maybeFinishSyntheticExpansion(expansion: PendingSyntheticExpansion) {
        if (expansion.pending > 0 || expansion.count === undefined) {
            return;
        }

        const variables: DapVariable[] = [
            {
                name: "count",
                type: "usize",
                value: String(expansion.count),
                variablesReference: 0,
            },
            {
                name: "capacity",
                type: "usize",
                value: expansion.capacity ?? "<unavailable>",
                variablesReference: 0,
            },
            {
                name: "data",
                type: `${expansion.array.itemType} *`,
                value: expansion.data ?? "<unavailable>",
                variablesReference: 0,
            },
        ];

        const itemLimit = Math.min(expansion.count, 100);
        for (let i = 0; i < itemLimit; ++i) {
            variables.push(
                expansion.itemResults.get(i) ?? {
                    name: `[${i}]`,
                    type: expansion.array.itemType,
                    value: "<unavailable>",
                    variablesReference: 0,
                }
            );
        }
        if (expansion.count > itemLimit) {
            variables.push({
                name: "...",
                value: `${expansion.count - itemLimit} more element(s)`,
                variablesReference: 0,
            });
        }

        this.pendingSyntheticExpansions.delete(expansion.originalRequest.seq ?? 0);
        this.emitter.fire({
            seq: this.nextInternalSeq++,
            type: "response",
            request_seq: expansion.originalRequest.seq,
            success: true,
            command: "variables",
            body: { variables },
        } as DapMessage);
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
