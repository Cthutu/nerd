import * as vscode from "vscode";
import * as fs from "fs";
import * as path from "path";
import {
    Executable,
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
} from "vscode-languageclient/node";

let client: LanguageClient | undefined;
let outputChannel: vscode.OutputChannel | undefined;

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

function getServerExecutable(): Executable {
    const config = vscode.workspace.getConfiguration("nerd");
    const configuredPath = config.get<string>("languageServer.path", "").trim();
    const args = config.get<string[]>("languageServer.args", ["lsp"]);
    const command =
        configuredPath || findWorkspaceServer() || findUserServer() || "nerd";

    return { command, args };
}

export function activate(
    context: vscode.ExtensionContext
): Thenable<void> | undefined {
    outputChannel = vscode.window.createOutputChannel("Nerd Language Server");
    context.subscriptions.push(outputChannel);

    const serverExecutable = getServerExecutable();
    outputChannel.appendLine(
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

    client = new LanguageClient(
        "nerdLanguageServer",
        "Nerd Language Server",
        serverOptions,
        clientOptions
    );

    context.subscriptions.push(client);
    return client.start();
}

export function deactivate(): Thenable<void> | undefined {
    outputChannel?.appendLine("Stopping server");
    return client?.stop();
}
