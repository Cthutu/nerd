#!/usr/bin/env python3
from __future__ import annotations

import argparse
import difflib
import json
import os
import pathlib
import re
import shlex
import subprocess
import sys
from urllib.parse import quote
from dataclasses import dataclass

if os.name == "nt":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")

try:
    from rich.console import Console
except ImportError:  # pragma: no cover - fallback for minimal Python installs
    Console = None


ROOT = pathlib.Path(__file__).resolve().parents[1]
NERD = ROOT / "_bin" / ("nerd-debug.exe" if os.name == "nt" else "nerd-debug")
DELIM = "\n¬\n"


@dataclass
class Failure:
    path: pathlib.Path
    message: str


@dataclass
class SuiteCounts:
    total: int = 0
    passed: int = 0
    failed: int = 0
    skipped: int = 0


console = Console() if Console else None

ANSI_RESET = "\033[0m"
ANSI_BOLD_WHITE = "\033[1;37m"
ANSI_BG_BLUE = "\033[44m"
ANSI_FAINT_WHITE = "\033[2;37m"
ANSI_RED = "\033[31m"
ANSI_GREEN = "\033[32m"
ANSI_CYAN = "\033[36m"
ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")

SUITE_LABELS = {
    "language": "language",
    "errors": "error",
    "hir": "hir",
    "llvm": "llvm",
    "format": "format",
    "lsp": "lsp",
    "commands": "command",
}


def rel(path: pathlib.Path) -> str:
    return path.relative_to(ROOT).as_posix()


def lsp_repo_uri() -> str:
    path = quote(ROOT.as_posix(), safe="/")
    return f"file:///{path}" if os.name == "nt" else f"file://{path}"


def normalize_repo_uris(text: str) -> str:
    return text.replace("__REPO_URI__", lsp_repo_uri()).replace(
        "file:///home/matt/nerd", lsp_repo_uri()
    )


def normalize_repo_paths(text: str) -> str:
    return text.replace(ROOT.as_posix(), "__REPO__").replace(str(ROOT), "__REPO__").replace("\\", "/")


def llvm_escaped_path(path: pathlib.Path) -> str:
    return str(path).replace("\\", "\\5C")


def normalize_assert_source_path_lengths(text: str) -> str:
    pattern = r'(@\.assert\.source_path\.m\d+\.\d+ = .* constant )\[\d+ x i8\] c"([^"]*)\\00"'

    def replace(match: re.Match[str]) -> str:
        count = len(match.group(2)) + 1
        return f'{match.group(1)}[{count} x i8] c"{match.group(2)}\\00"'

    return re.sub(pattern, replace, text)


def normalized_returncode(code: int) -> int:
    return code & 0xFF if os.name == "nt" and code > 255 else code


def current_platform() -> str:
    return "windows" if os.name == "nt" else "linux"


def case_platforms(path: pathlib.Path) -> set[str]:
    text = path.read_text(encoding="utf-8")
    source = text.split("¬", 1)[0]
    for line in source.splitlines():
        match = re.match(r"\s*--\s*test-platforms?\s*:\s*(.+)$", line)
        if match:
            return {item.strip().lower() for item in re.split(r"[,\s]+", match.group(1)) if item.strip()}
    return set()


def colour(text: str, code: str) -> str:
    if console:
        return text
    return f"\033[{code}m{text}\033[0m"


def out(text: str) -> None:
    if console:
        console.print(text, highlight=False, markup=False)
    else:
        print(text)


def emit(text: str) -> None:
    if console:
        console.out(text, highlight=False)
    else:
        print(text)


def split_sections(path: pathlib.Path) -> list[str]:
    sections = path.read_text(encoding="utf-8").split("¬")
    normalized: list[str] = []
    for section in sections:
        if section.startswith("\r\n"):
            section = section[2:]
        elif section.startswith("\n"):
            section = section[1:]
        if section.endswith("\r\n"):
            section = section[:-2]
        elif section.endswith("\n"):
            section = section[:-1]
        normalized.append(section)
    return normalized


def norm(text: str) -> str:
    return text.replace("\r\n", "\n")


def strip_ansi(text: str) -> str:
    return ANSI_RE.sub("", text)


def unified(expected: str, actual: str, expected_name: str, actual_name: str) -> str:
    return "".join(
        difflib.unified_diff(
            norm(expected).splitlines(keepends=True),
            norm(actual).splitlines(keepends=True),
            fromfile=expected_name,
            tofile=actual_name,
        )
    )


def env() -> dict[str, str]:
    result = os.environ.copy()
    test_mods = str(ROOT / "tests" / "mods")
    mods = str(ROOT / "mods")
    existing = result.get("NERD_LIB_PATH")
    result["NERD_LIB_PATH"] = (
        os.pathsep.join([test_mods, mods, existing]) if existing else os.pathsep.join([test_mods, mods])
    )
    return result


def run_cmd(
    args: list[str],
    *,
    cwd: pathlib.Path = ROOT,
    extra_env: dict[str, str] | None = None,
    stdin: str | None = None,
) -> subprocess.CompletedProcess[str]:
    merged_env = env()
    if extra_env:
        merged_env.update(extra_env)
    return subprocess.run(
        args,
        cwd=cwd,
        env=merged_env,
        text=True,
        encoding="utf-8",
        errors="replace",
        input=stdin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def check_equal(path: pathlib.Path, label: str, expected: str, actual: str) -> Failure | None:
    if norm(expected) == norm(actual):
        return None
    return Failure(path, f"{label} mismatch\n{unified(expected, actual, label + '.expected', label + '.actual')}")


def lines_are_subsequence(expected: str, actual: str, *, strip_dollars: bool = False) -> bool:
    expected_lines = [line.strip() for line in norm(expected).splitlines() if line.strip()]
    actual_lines = [line.strip() for line in norm(actual).splitlines() if line.strip()]
    if strip_dollars:
        expected_lines = [line.replace("$", "") for line in expected_lines]
        actual_lines = [line.replace("$", "") for line in actual_lines]
    expected_lines = [line for line in expected_lines if line != "void init() {}"]
    index = 0
    for line in actual_lines:
        current = line.replace("$", "") if strip_dollars else line
        if index < len(expected_lines) and current == expected_lines[index]:
            index += 1
    return index == len(expected_lines)


def cleanup_generated_outputs(path: pathlib.Path) -> None:
    for sidecar in (
        path.parent / path.stem,
        path.parent / f"{path.stem}.exe",
        path.parent / f"{path.stem}.input",
        path.parent / f"{path.stem}.input.exe",
        path.parent / f"{path.stem}.input.pdb",
        path.parent / f"{path.stem}.link.ll",
        path.parent / f"{path.stem}.exe.link.ll",
        path.parent / f"{path.stem}.input.link.ll",
        path.parent / f"{path.stem}.input.exe.link.ll",
    ):
        if sidecar.is_file():
            sidecar.unlink(missing_ok=True)
    for pattern in (
        f"_{path.stem}*",
        f"__{path.stem}*",
        f"{path.stem}.m*.ll",
        f"{path.stem}.exe.m*.ll",
        f"{path.stem}.input.m*.ll",
        f"{path.stem}.input.exe.m*.ll",
        f"_{path.stem}.input*",
        f"__{path.stem}.input*",
    ):
        for sidecar in path.parent.glob(pattern):
            if sidecar.is_file():
                sidecar.unlink(missing_ok=True)


def test_language(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) < 5:
        return [Failure(path, "language test must have source, exit, stdout, HIR, and LLVM sections")]

    source, expected_exit, expected_stdout, expected_hir, expected_llvm = parts[:5]
    stdin = parts[5] if len(parts) > 5 else None
    input_path = path.with_suffix(".input.n")
    input_path.write_text(source, encoding="utf-8", newline="\n")
    cleanup_generated_outputs(path)

    proc = run_cmd(
        [
            str(NERD),
            "run",
            "--hir",
            "--llvm",
            str(input_path),
        ],
        stdin=stdin,
    )

    failures: list[Failure] = []
    expected_code = int(expected_exit.strip() or "0")
    actual_code = normalized_returncode(proc.returncode)
    if actual_code != expected_code:
        failures.append(Failure(path, f"exit mismatch: expected {expected_code}, got {proc.returncode}\n{proc.stderr}"))

    if expected_stdout.rstrip("\n") != proc.stdout.rstrip("\n"):
        stdout_failure = check_equal(path, "stdout", expected_stdout, proc.stdout)
        if stdout_failure:
            failures.append(stdout_failure)

    input_variants = {
        str(input_path),
        str(input_path).replace("\\", "\\\\"),
        llvm_escaped_path(input_path),
        input_path.as_posix(),
    }

    hir_path = path.parent / f"_{path.stem}.hir"
    if expected_hir.strip():
        actual_hir = hir_path.read_text(encoding="utf-8") if hir_path.exists() else ""
        for input_variant in input_variants:
            actual_hir = actual_hir.replace(input_variant, rel(path))
        if not lines_are_subsequence(expected_hir, actual_hir):
            hir_failure = check_equal(path, "hir", expected_hir, actual_hir)
            if hir_failure:
                failures.append(hir_failure)

    llvm_path = path.parent / f"_{path.stem}.ll"
    if expected_llvm.strip():
        expected_llvm = normalize_assert_source_path_lengths(expected_llvm)
        actual_llvm = llvm_path.read_text(encoding="utf-8") if llvm_path.exists() else ""
        for input_variant in input_variants:
            actual_llvm = actual_llvm.replace(input_variant, rel(path))
        actual_llvm = normalize_assert_source_path_lengths(actual_llvm)
        if not lines_are_subsequence(expected_llvm, actual_llvm):
            llvm_failure = check_equal(path, "llvm", expected_llvm, actual_llvm)
            if llvm_failure:
                failures.append(llvm_failure)

    if not failures:
        input_path.unlink(missing_ok=True)
        cleanup_generated_outputs(path)
    return failures


def test_hir(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) != 2:
        return [Failure(path, "HIR test must have source and expected HIR sections")]

    source, expected_hir = parts
    input_path = path.with_suffix(".input.n")
    output_root = path.parent / path.stem
    hir_path = path.parent / f"_{path.stem}.hir"

    input_path.write_text(source, encoding="utf-8", newline="\n")
    cleanup_generated_outputs(path)

    proc = run_cmd([
        str(NERD),
        "build",
        "--hir",
        str(input_path),
        "--output",
        str(output_root),
    ])

    failures: list[Failure] = []
    if proc.returncode != 0:
        failures.append(Failure(path, f"build failed with {proc.returncode}\n{proc.stderr}"))
    else:
        actual_hir = hir_path.read_text(encoding="utf-8") if hir_path.exists() else ""
        if not lines_are_subsequence(expected_hir, actual_hir):
            hir_failure = check_equal(path, "hir", expected_hir, actual_hir)
            if hir_failure:
                failures.append(hir_failure)

    if not failures:
        input_path.unlink(missing_ok=True)
        cleanup_generated_outputs(path)
        output_root.unlink(missing_ok=True)
    return failures


def test_llvm(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) != 2:
        return [Failure(path, "LLVM test must have source and expected LLVM sections")]

    source, expected_llvm = parts
    input_path = path.with_suffix(".input.n")
    output_root = path.parent / path.stem
    llvm_path = path.parent / f"_{path.stem}.ll"

    input_path.write_text(source, encoding="utf-8", newline="\n")
    cleanup_generated_outputs(path)

    proc = run_cmd([
        str(NERD),
        "build",
        "--llvm",
        str(input_path),
        "--output",
        str(output_root),
    ])

    failures: list[Failure] = []
    if proc.returncode != 0:
        failures.append(Failure(path, f"build failed with {proc.returncode}\n{proc.stderr}"))
    else:
        input_variants = {
            str(input_path),
            str(input_path).replace("\\", "\\\\"),
            llvm_escaped_path(input_path),
            input_path.as_posix(),
            f"/home/matt/nerd/{rel(input_path)}",
        }
        actual_llvm = llvm_path.read_text(encoding="utf-8") if llvm_path.exists() else ""
        for input_variant in input_variants:
            expected_llvm = expected_llvm.replace(input_variant, rel(input_path))
            actual_llvm = actual_llvm.replace(input_variant, rel(input_path))
        expected_llvm = normalize_assert_source_path_lengths(expected_llvm)
        actual_llvm = normalize_assert_source_path_lengths(actual_llvm)
        if not lines_are_subsequence(expected_llvm, actual_llvm):
            llvm_failure = check_equal(path, "llvm", expected_llvm, actual_llvm)
            if llvm_failure:
                failures.append(llvm_failure)

    if not failures:
        input_path.unlink(missing_ok=True)
        cleanup_generated_outputs(path)
        output_root.unlink(missing_ok=True)
        input_path.with_suffix("").unlink(missing_ok=True)
    return failures


def normalize_error_json(text: str, source_file: str) -> str:
    parsed = json.loads(text)
    parsed["source_file"] = source_file
    return json.dumps(parsed, indent=4, sort_keys=False) + "\n"


def test_errors(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if parts and parts[-1] == "":
        parts = parts[:-1]
    if len(parts) % 2 != 0:
        return [Failure(path, "error tests must contain source/expected pairs")]

    failures: list[Failure] = []
    for index in range(0, len(parts), 2):
        source = parts[index].rstrip("\n")
        expected = parts[index + 1]
        input_path = path.with_suffix(f".{index // 2}.input.n")
        input_path.write_text(source, encoding="utf-8", newline="\n")
        proc = run_cmd(
            [str(NERD), "build", str(input_path)],
            extra_env={"NERD_ERROR_RENDER_TEST": "1"},
        )
        actual = proc.stdout if proc.stdout.strip() else proc.stderr
        if proc.returncode == 0:
            failures.append(Failure(path, f"expected compiler error for case {index // 2}, got success"))
            continue
        try:
            actual = normalize_error_json(actual, rel(path))
        except Exception as exc:  # noqa: BLE001 - report malformed compiler output
            failures.append(Failure(path, f"case {index // 2} did not produce JSON diagnostics: {exc}\n{actual}"))
            continue
        try:
            expected_obj = json.loads(expected)
            actual_obj = json.loads(actual)
        except Exception:
            failure = check_equal(path, f"error case {index // 2}", expected, actual)
            if failure:
                failures.append(failure)
            continue
        if expected_obj != actual_obj:
            failure = check_equal(path, f"error case {index // 2}", expected, actual)
            if failure:
                failures.append(failure)
    if not failures:
        for input_path in path.parent.glob(f"{path.stem}.*.input.n"):
            input_path.unlink(missing_ok=True)
    return failures


def test_format(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) != 2:
        return [Failure(path, "format test must have source and expected sections")]
    source, expected = parts
    input_path = path.with_suffix(".input.n")
    input_path.write_text(source, encoding="utf-8", newline="\n")
    proc = run_cmd([str(NERD), "format", "--stdout", str(input_path)])
    if proc.returncode != 0:
        return [Failure(path, f"formatter failed with exit {proc.returncode}\n{proc.stderr}")]
    failure = check_equal(path, "format", expected, proc.stdout)
    if expected.rstrip("\n") == proc.stdout.rstrip("\n"):
        failure = None
    if failure:
        return [failure]

    idempotence_path = path.with_suffix(".idempotence.input.n")
    idempotence_path.write_text(proc.stdout, encoding="utf-8", newline="\n")
    idempotence_proc = run_cmd([str(NERD), "format", "--stdout", str(idempotence_path)])
    if idempotence_proc.returncode != 0:
        input_path.unlink(missing_ok=True)
        idempotence_path.unlink(missing_ok=True)
        return [
            Failure(
                path,
                f"formatter idempotence failed with exit {idempotence_proc.returncode}\n"
                f"{idempotence_proc.stderr}",
            )
        ]
    idempotence_failure = check_equal(
        path, "format idempotence", proc.stdout, idempotence_proc.stdout
    )
    if idempotence_failure:
        input_path.unlink(missing_ok=True)
        idempotence_path.unlink(missing_ok=True)
        return [idempotence_failure]

    input_path.unlink(missing_ok=True)
    idempotence_path.unlink(missing_ok=True)
    return []


def lsp_frame(payload: dict) -> bytes:
    data = json.dumps(payload).encode()
    return f"Content-Length: {len(data)}\r\n\r\n".encode() + data


def lsp_read_frames(data: bytes) -> list[dict]:
    frames: list[dict] = []
    offset = 0
    while offset < len(data):
        header_end = data.find(b"\r\n\r\n", offset)
        if header_end < 0:
            break
        header = data[offset:header_end].decode()
        length = 0
        for line in header.split("\r\n"):
            if line.lower().startswith("content-length:"):
                length = int(line.split(":", 1)[1].strip())
        start = header_end + 4
        end = start + length
        frames.append(json.loads(data[start:end]))
        offset = end
    return frames


def test_lsp(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) != 3:
        return [Failure(path, "LSP test must have source, requests, and expected sections")]
    source, requests_text, expected_text = parts
    requests = json.loads(normalize_repo_uris(requests_text))

    uri = "file:///test.n"
    messages = [
        {
            "jsonrpc": "2.0",
            "id": 1,
            "method": "initialize",
            "params": {"rootUri": None, "capabilities": {}},
        },
        {
            "jsonrpc": "2.0",
            "method": "textDocument/didOpen",
            "params": {
                "textDocument": {
                    "uri": uri,
                    "languageId": "nerd",
                    "version": 1,
                    "text": source,
                }
            },
        },
        *requests,
        {"jsonrpc": "2.0", "id": 999, "method": "shutdown", "params": None},
        {"jsonrpc": "2.0", "method": "exit", "params": None},
    ]
    input_bytes = b"".join(lsp_frame(message) for message in messages)
    proc = subprocess.run(
        [str(NERD), "lsp"],
        cwd=ROOT,
        env=env(),
        input=input_bytes,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if proc.returncode != 0:
        return [Failure(path, f"LSP failed with exit {proc.returncode}\n{proc.stderr.decode(errors='replace')}")]
    actual = json.dumps(lsp_read_frames(proc.stdout), indent=4) + "\n"
    expected_text = normalize_repo_uris(expected_text).replace("/_bin/mods/", "/mods/")
    try:
        if json.loads(expected_text) == json.loads(actual):
            return []
    except Exception:
        pass
    failure = check_equal(path, "lsp", expected_text, actual)
    return [failure] if failure else []


def test_command(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) < 3:
        return [Failure(path, "command test must have source, exit, and stdout sections")]

    source = parts[0]
    expected_exit = int(parts[1].strip() or "0")
    expected_stdout = parts[2]
    run_mode = parts[3].strip() if len(parts) > 3 else "delete"
    cli_args = shlex.split(parts[4].strip()) if len(parts) > 4 and parts[4].strip() else []
    command = parts[5].strip() if len(parts) > 5 and parts[5].strip() else "run"
    expected_stderr = parts[6] if len(parts) > 6 else ""

    cwd = path.parent
    input_path = cwd / f"{path.stem}.input.n"
    input_path.write_text(source, encoding="utf-8", newline="\n")
    executable = input_path.with_suffix("")
    if command in {"build", "b"} and current_platform() == "windows":
        executable = pathlib.Path(f"{executable}.exe")

    if run_mode == "clean-llvm" and command in {"run", "r"}:
        for stale in (
            cwd / f"_{path.stem}.out.link.ll",
            cwd / f"_{path.stem}.out.nrt.o",
            cwd / f"_{path.stem}.out.m1.ll",
        ):
            stale.write_text("stale generated artifact\n", encoding="utf-8")
        if expected_exit != 0:
            binary_suffix = ".exe" if current_platform() == "windows" else ""
            build_stem = input_path.with_suffix("").name
            for stale in (
                cwd / f"{build_stem}{binary_suffix}.link.ll",
                cwd / f"{build_stem}{binary_suffix}.nrt.o",
                cwd / f"{build_stem}{binary_suffix}.m1.ll",
                cwd / f"_{path.stem}.hir",
                cwd / f"_{path.stem}.ll",
            ):
                stale.write_text("stale generated artifact\n", encoding="utf-8")

    args = [str(NERD), command, *cli_args]
    if command in {"run", "r"} and run_mode == "keep" and "--keep" not in args:
        args.append("--keep")
    if command not in {"explain", "internal-test"}:
        args.append(str(input_path.name))
    proc = run_cmd(args, cwd=cwd)

    failures: list[Failure] = []
    actual_exit = normalized_returncode(proc.returncode)
    if actual_exit != expected_exit:
        failures.append(Failure(path, f"exit mismatch: expected {expected_exit}, got {proc.returncode}\n{proc.stderr}"))
    if expected_stdout.strip():
        actual_stdout = strip_ansi(proc.stdout)
        if expected_stdout.rstrip("\n") != actual_stdout.rstrip("\n"):
            stdout_failure = check_equal(path, "stdout", expected_stdout, actual_stdout)
            if stdout_failure:
                failures.append(stdout_failure)
    if expected_stderr.strip():
        actual_stderr = normalize_repo_paths(strip_ansi(proc.stderr))
        expected_stderr = normalize_repo_paths(expected_stderr)
        if expected_stderr.rstrip("\n") != actual_stderr.rstrip("\n"):
            stderr_failure = check_equal(path, "stderr", expected_stderr, actual_stderr)
            if stderr_failure:
                failures.append(stderr_failure)

    debug_symbols = executable.with_suffix(".pdb")
    if run_mode == "keep" and not executable.exists():
        failures.append(Failure(path, "expected command to keep generated executable"))
    if run_mode in {"delete", "clean-llvm"} and executable.exists():
        failures.append(Failure(path, "expected command to delete generated executable"))
        executable.unlink()
    if run_mode == "clean-llvm":
        leftovers: list[pathlib.Path] = []
        for pattern in (
            f"{executable.name}*.ll",
            f"{executable.name}*.nrt.o",
            f"_{path.stem}.out*.ll",
            f"_{path.stem}.out*.nrt.o",
            f"{path.stem}.exe*.ll",
            f"{path.stem}.exe*.nrt.o",
            f"{path.stem}*.link.ll",
            f"{path.stem}*.nrt.o",
            f"{path.stem}.m*.ll",
            f"_{input_path.stem}*.ll",
            f"_{input_path.stem}*.nrt.o",
            f"{input_path.stem}.m*.ll",
        ):
            leftovers.extend(sorted(cwd.glob(pattern)))
        if expected_exit != 0:
            build_stem = input_path.with_suffix("").name
            leftovers.extend([
                cwd / f"_{path.stem}.hir",
                cwd / f"_{path.stem}.ll",
            ])
        leftovers = [item for item in leftovers if item.is_file()]
        if leftovers:
            names = ", ".join(item.name for item in leftovers)
            failures.append(Failure(path, f"expected LLVM run to clean generated files, found: {names}"))
    if not failures:
        input_path.unlink(missing_ok=True)
        executable.unlink(missing_ok=True)
        debug_symbols.unlink(missing_ok=True)
    return failures


def collect() -> list[tuple[str, pathlib.Path]]:
    cases: list[tuple[str, pathlib.Path]] = []
    for kind, directory, suffix in [
        ("language", "tests/language", "*.t"),
        ("errors", "tests/errors", "*.e"),
        ("hir", "tests/hir", "*.hir"),
        ("llvm", "tests/llvm", "*.ll"),
        ("format", "tests/format", "*.f"),
        ("lsp", "tests/lsp", "*.lsp"),
        ("commands", "tests/commands", "*.cmd"),
    ]:
        for path in sorted((ROOT / directory).glob(suffix)):
            if kind in {"hir", "llvm"} and path.name.startswith("_"):
                continue
            if kind == "llvm" and re.search(r"(\.input)?\.m\d+\.ll$", path.name):
                continue
            cases.append((kind, path))
    return cases


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Nerd compiler regression tests")
    parser.add_argument("--filter", default="", help="Only run test paths containing this text")
    args = parser.parse_args()

    runners = {
        "language": test_language,
        "errors": test_errors,
        "hir": test_hir,
        "llvm": test_llvm,
        "format": test_format,
        "lsp": test_lsp,
        "commands": test_command,
    }

    cases = [(kind, path) for kind, path in collect() if args.filter in rel(path)]
    counts: dict[str, SuiteCounts] = {kind: SuiteCounts() for kind in runners}
    failures: list[Failure] = []
    for kind, path in cases:
        counts[kind].total += 1
        platforms = case_platforms(path)
        if platforms and current_platform() not in platforms:
            counts[kind].skipped += 1
            print_result_line(None, SUITE_LABELS[kind], rel(path))
            continue
        case_failures = runners[kind](path)
        if case_failures:
            counts[kind].failed += 1
            print_result_line(False, SUITE_LABELS[kind], rel(path))
            for failure in case_failures:
                out(failure.message)
            failures.extend(case_failures)
        else:
            counts[kind].passed += 1
            print_result_line(True, SUITE_LABELS[kind], rel(path))

    print_summary(counts)
    return 1 if failures else 0


def print_result_line(passed: bool | None, kind: str, label: str) -> None:
    colour_code = ANSI_GREEN if passed else ANSI_RED
    status = "PASS" if passed else "FAIL"
    if passed is None:
        colour_code = ANSI_CYAN
        status = "SKIP"
    emit(f"{colour_code}[{status}]{ANSI_RESET} {kind}: {label}")


def visible_width(text: str) -> int:
    return len(text)


def print_repeat(text: str, count: int) -> None:
    if count <= 0:
        return
    if console:
        console.out(text * count, end="", highlight=False)
    else:
        print(text * count, end="")


def print_box_line(left: str, mid: str, right: str, widths: list[int]) -> None:
    if console:
        console.out(f"{ANSI_FAINT_WHITE}{left}", end="", highlight=False)
        for index, width in enumerate(widths):
            console.out("─" * (width + 2), end="", highlight=False)
            if index + 1 < len(widths):
                console.out(mid, end="", highlight=False)
        console.out(f"{right}{ANSI_RESET}", highlight=False)
        return

    print(f"{ANSI_FAINT_WHITE}{left}", end="")
    for index, width in enumerate(widths):
        print("─" * (width + 2), end="")
        if index + 1 < len(widths):
            print(mid, end="")
    print(f"{right}{ANSI_RESET}")


def print_span_line(left: str, right: str, width: int) -> None:
    emit(f"{ANSI_FAINT_WHITE}{left}{'─' * width}{right}{ANSI_RESET}")


def print_text_cell(text: str, width: int, colour_code: str) -> str:
    padding = max(0, width - visible_width(text))
    return f" {colour_code}{text}{ANSI_RESET}{' ' * padding} "


def print_number_cell(value: int, width: int, colour_code: str) -> str:
    text = str(value)
    padding = max(0, width - len(text))
    return f" {colour_code}{text}{ANSI_RESET}{' ' * padding} "


def print_table_row(cells: list[str], widths: list[int], colours: list[str], numeric: list[bool]) -> None:
    parts = [f"{ANSI_FAINT_WHITE}│{ANSI_RESET}"]
    for value, width, colour_code, is_numeric in zip(cells, widths, colours, numeric):
        if is_numeric:
            parts.append(print_number_cell(int(value), width, colour_code))
        else:
            parts.append(print_text_cell(value, width, colour_code))
        parts.append(f"{ANSI_FAINT_WHITE}│{ANSI_RESET}")
    emit("".join(parts))


def print_summary(counts: dict[str, SuiteCounts]) -> None:
    total = SuiteCounts()
    for count in counts.values():
        total.total += count.total
        total.passed += count.passed
        total.failed += count.failed
        total.skipped += count.skipped

    headers = ["Type", "Passed", "Failed", "Skipped"]
    rows = [
        (SUITE_LABELS[kind], count.passed, count.failed, count.skipped)
        for kind, count in counts.items()
    ]
    rows.append(("total", total.passed, total.failed, total.skipped))

    widths = [
        max(visible_width(headers[0]), *(visible_width(row[0]) for row in rows)),
        max(visible_width(headers[1]), *(len(str(row[1])) for row in rows)),
        max(visible_width(headers[2]), *(len(str(row[2])) for row in rows)),
        max(visible_width(headers[3]), *(len(str(row[3])) for row in rows)),
    ]
    content_width = sum(width + 2 for width in widths) + len(widths) - 1
    title = "Test Summary"
    title_width = visible_width(title)
    if content_width < title_width + 2:
        widths[-1] += title_width + 2 - content_width
        content_width = title_width + 2

    emit("")
    print_span_line("┌", "┐", content_width)
    title_padding = max(0, content_width - title_width - 1)
    emit(
        f"{ANSI_FAINT_WHITE}│{ANSI_RESET}"
        f"{ANSI_BG_BLUE}{ANSI_BOLD_WHITE} {title}{' ' * title_padding}"
        f"{ANSI_RESET}{ANSI_FAINT_WHITE}│{ANSI_RESET}"
    )
    print_box_line("├", "┬", "┤", widths)
    print_table_row(headers, widths, [ANSI_BOLD_WHITE] * len(headers), [False] * len(headers))
    print_box_line("├", "┼", "┤", widths)
    for index, row in enumerate(rows):
        if index == len(rows) - 1:
            print_box_line("├", "┼", "┤", widths)
        print_table_row(
            [row[0], str(row[1]), str(row[2]), str(row[3])],
            widths,
            [ANSI_CYAN, ANSI_GREEN, ANSI_RED, ANSI_CYAN],
            [False, True, True, True],
        )
    print_box_line("└", "┴", "┘", widths)


if __name__ == "__main__":
    sys.exit(main())
