import * as vscode from "vscode";
import * as fs from "fs";
import * as os from "os";
import * as path from "path";
import { execFile } from "child_process";
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

function execFileAsync(
    command: string,
    args: string[]
): Promise<{ stdout: string; stderr: string }> {
    return new Promise((resolve, reject) => {
        execFile(command, args, (error, stdout, stderr) => {
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

function registerFormatter(context: vscode.ExtensionContext) {
    formatterRegistration?.dispose();
    formatterRegistration = vscode.languages.registerDocumentFormattingEditProvider(
        { scheme: "file", language: "nerd" },
        {
            async provideDocumentFormattingEdits(document) {
                const formattedText = await provideFormattedText(document);
                const lastLine = document.lineCount > 0 ? document.lineAt(document.lineCount - 1) : undefined;
                const fullRange = lastLine
                    ? new vscode.Range(0, 0, document.lineCount - 1, lastLine.text.length)
                    : new vscode.Range(0, 0, 0, 0);

                return [vscode.TextEdit.replace(fullRange, formattedText)];
            },
        }
    );
    context.subscriptions.push(formatterRegistration);
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
    return startLanguageServer(context);
}

export function deactivate(): Thenable<void> | undefined {
    outputChannel?.appendLine("Stopping server");
    return stopLanguageServer();
}
