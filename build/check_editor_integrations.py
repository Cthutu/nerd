#!/usr/bin/env python3
import argparse
import json
import os
import pathlib
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]


def resolve_nerd(value: str) -> str:
    path = pathlib.Path(value)
    if path.is_absolute():
        return str(path)
    if (ROOT / path).exists():
        return str((ROOT / path).resolve())
    resolved = shutil.which(value)
    if resolved is None:
        raise AssertionError(f"nerd executable not found: {value}")
    return resolved


def check_vscode_extension() -> None:
    package_path = ROOT / "syntax" / "nerd-vscode" / "package.json"
    package = json.loads(package_path.read_text(encoding="utf-8"))
    contributes = package["contributes"]

    language = contributes["languages"][0]
    if language["id"] != "nerd" or ".n" not in language["extensions"]:
        raise AssertionError("VS Code language contribution does not register .n as nerd")

    grammar_path = ROOT / "syntax" / "nerd-vscode" / contributes["grammars"][0]["path"]
    if not grammar_path.exists():
        raise AssertionError(f"VS Code grammar path is missing: {grammar_path}")

    breakpoints = contributes.get("breakpoints", [])
    if not any(item.get("language") == "nerd" for item in breakpoints):
        raise AssertionError("VS Code extension does not enable breakpoints for nerd")

    debuggers = contributes.get("debuggers", [])
    nerd_debugger = next(
        (debugger for debugger in debuggers if debugger.get("type") == "nerd"),
        None,
    )
    if nerd_debugger is None:
        raise AssertionError("VS Code extension is missing the nerd debugger type")
    if "nerd" not in nerd_debugger.get("languages", []):
        raise AssertionError("VS Code nerd debugger does not target nerd files")

    commands = {command["command"] for command in contributes["commands"]}
    for command in [
        "nerd.restartLanguageServer",
        "nerd.buildActiveFileForDebug",
        "nerd.debugActiveFileWithCodeLLDB",
    ]:
        if command not in commands:
            raise AssertionError(f"VS Code extension is missing command {command!r}")

    properties = contributes["configuration"]["properties"]
    args = properties["nerd.languageServer.args"]["default"]
    if args != ["lsp"]:
        raise AssertionError("VS Code language server default args must be ['lsp']")

    extension_source = (ROOT / "syntax" / "nerd-vscode" / "src" / "extension.ts").read_text(
        encoding="utf-8"
    )
    required_fragments = [
        "findWorkspaceServer()",
        "findUserServer()",
        'const command = sourcePath',
        "getServerEnvironment(sourcePath)",
        "registerEnterIndentation(context)",
        "computeNerdIndent(lines, line)",
        "nerd.restartLanguageServer",
        "nerd.buildActiveFileForDebug",
        "nerd.debugActiveFileWithCodeLLDB",
        'type: "nerd"',
        "registerDebugAdapterDescriptorFactory",
        "supportsDelayedStackTraceLoading",
        "normalizeNerdBreakpointRequest",
        "nearestNerdBreakpointLine",
        "Moved ${movedCount} Nerd breakpoint(s)",
        "dynamicArrayHeaderWatchExpression",
        "collectDynamicArrayDeclarationsFromText",
        "collectNerdDeclarationsNearPath",
        "syntheticDynamicArrays",
        "syntheticEnums",
        'expressions: message.arguments?.expressions ?? "native"',
        "pendingDynamicArrayRetries",
        "Nerd Globals",
        '["build", "--output", outputPath, sourcePath]',
        "fullDocumentRange(document)",
        "document.positionAt(document.getText().length)",
        "suppressEnterIndentUntil",
        r"/^\r?\n[ \t]*$/.test(change.text)",
    ]
    for fragment in required_fragments:
        if fragment not in extension_source:
            raise AssertionError(f"VS Code extension is missing {fragment!r}")


def check_nvim_files() -> None:
    nvim_root = ROOT / "syntax" / "nerd-nvim"
    required = [
        nvim_root / "lua" / "plugins" / "nerd.lua",
        nvim_root / "ftdetect" / "nerd.vim",
        nvim_root / "indent" / "nerd.vim",
        nvim_root / "syntax" / "nerd.vim",
    ]
    for path in required:
        if not path.exists():
            raise AssertionError(f"Neovim integration file is missing: {path}")

    plugin = required[0].read_text(encoding="utf-8")
    for fragment in [
        'cmd = { "nerd", "lsp" }',
        'filetypes = { "nerd" }',
        'opts.formatters_by_ft.nerd = { "nerd" }',
        'args = { "format", "$FILENAME" }',
    ]:
        if fragment not in plugin:
            raise AssertionError(f"Neovim plugin is missing {fragment!r}")

    ftdetect = required[1].read_text(encoding="utf-8")
    if "*.n setfiletype nerd" not in ftdetect:
        raise AssertionError("Neovim ftdetect does not map .n to nerd")

    indent = required[2].read_text(encoding="utf-8")
    for fragment in [
        "setlocal indentexpr=GetNerdIndent(v:lnum)",
        "function! GetNerdIndent(lnum) abort",
    ]:
        if fragment not in indent:
            raise AssertionError(f"Neovim indent file is missing {fragment!r}")


def lsp_message(payload: dict) -> bytes:
    data = json.dumps(payload, separators=(",", ":")).encode("utf-8")
    return b"Content-Length: " + str(len(data)).encode("ascii") + b"\r\n\r\n" + data


def read_lsp_message(proc: subprocess.Popen[bytes]) -> dict:
    headers: dict[str, str] = {}
    while True:
        line = proc.stdout.readline()
        if line == b"":
            stderr = proc.stderr.read().decode("utf-8", errors="replace")
            raise AssertionError(f"LSP server closed stdout\nstderr:\n{stderr}")
        if line in (b"\r\n", b"\n"):
            break
        key, _, value = line.decode("ascii").partition(":")
        headers[key.lower()] = value.strip()

    length = int(headers.get("content-length", "0"))
    if length <= 0:
        raise AssertionError("LSP response had no Content-Length")
    body = proc.stdout.read(length)
    return json.loads(body.decode("utf-8"))


def check_lsp_startup(nerd_cmd: str) -> None:
    env = os.environ.copy()
    env.setdefault("NERD_LIB_PATH", str(ROOT / "mods"))
    proc = subprocess.Popen(
        [nerd_cmd, "lsp"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
    )
    assert proc.stdin is not None
    try:
        proc.stdin.write(
            lsp_message(
                {
                    "jsonrpc": "2.0",
                    "id": 1,
                    "method": "initialize",
                    "params": {
                        "processId": None,
                        "rootUri": ROOT.as_uri(),
                        "capabilities": {},
                    },
                }
            )
        )
        proc.stdin.flush()
        response = read_lsp_message(proc)
        if response.get("id") != 1 or "result" not in response:
            raise AssertionError(f"unexpected LSP initialize response: {response}")

        proc.stdin.write(lsp_message({"jsonrpc": "2.0", "id": 2, "method": "shutdown"}))
        proc.stdin.write(lsp_message({"jsonrpc": "2.0", "method": "exit"}))
        proc.stdin.flush()
        shutdown = read_lsp_message(proc)
        if shutdown.get("id") != 2:
            raise AssertionError(f"unexpected LSP shutdown response: {shutdown}")
    finally:
        try:
            proc.stdin.close()
        except Exception:
            pass
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            raise AssertionError("LSP server did not exit after shutdown")


def main() -> int:
    parser = argparse.ArgumentParser(description="Check editor integration wiring")
    parser.add_argument(
        "--nerd",
        default=os.environ.get("NERD_BIN", shutil.which("nerd") or "nerd"),
        help="nerd executable used for the LSP startup check",
    )
    args = parser.parse_args()

    nerd_cmd = resolve_nerd(args.nerd)
    check_vscode_extension()
    check_nvim_files()
    check_lsp_startup(nerd_cmd)
    print("editor integration checks passed")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as exc:
        print(str(exc), file=sys.stderr)
        raise SystemExit(1)
