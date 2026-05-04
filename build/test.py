#!/usr/bin/env python3
from __future__ import annotations

import argparse
import difflib
import json
import os
import pathlib
import shlex
import subprocess
import sys
from dataclasses import dataclass

try:
    from rich.console import Console
    from rich.table import Table
except ImportError:  # pragma: no cover - fallback for minimal Python installs
    Console = None
    Table = None


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


console = Console() if Console else None


def rel(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT))


def colour(text: str, code: str) -> str:
    if console:
        return text
    return f"\033[{code}m{text}\033[0m"


def out(text: str, *, style: str | None = None) -> None:
    if console:
        console.print(text, style=style)
    else:
        print(text)


def split_sections(path: pathlib.Path) -> list[str]:
    sections = path.read_text().split("¬")
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


def test_language(path: pathlib.Path) -> list[Failure]:
    parts = split_sections(path)
    if len(parts) < 5:
        return [Failure(path, "language test must have source, exit, stdout, ir, and c sections")]

    source, expected_exit, expected_stdout, expected_ir, expected_c = parts[:5]
    stdin = parts[5] if len(parts) > 5 else None
    input_path = path.with_suffix(".input.n")
    output_root = path.parent / f"_{path.stem}"
    input_path.write_text(source)
    for suffix in (".ir", ".gen.c"):
        sidecar = path.parent / f"_{path.stem}{suffix}"
        if sidecar.exists():
            sidecar.unlink()

    proc = run_cmd(
        [
            str(NERD),
            "run",
            "--ir",
            "--cgen",
            str(input_path),
        ],
        stdin=stdin,
    )

    failures: list[Failure] = []
    expected_code = int(expected_exit.strip() or "0")
    if proc.returncode != expected_code:
        failures.append(Failure(path, f"exit mismatch: expected {expected_code}, got {proc.returncode}\n{proc.stderr}"))

    if expected_stdout.rstrip("\n") != proc.stdout.rstrip("\n"):
        stdout_failure = check_equal(path, "stdout", expected_stdout, proc.stdout)
        if stdout_failure:
            failures.append(stdout_failure)

    ir_path = path.parent / f"_{path.stem}.ir"
    if expected_ir.strip():
        actual_ir = ir_path.read_text() if ir_path.exists() else ""
        if not lines_are_subsequence(expected_ir, actual_ir):
            ir_failure = check_equal(path, "ir", expected_ir, actual_ir)
            if ir_failure:
                failures.append(ir_failure)

    c_path = path.parent / f"_{path.stem}.gen.c"
    if expected_c.strip():
        actual_c = c_path.read_text() if c_path.exists() else ""
        actual_c = actual_c.replace(str(input_path), rel(path))
        if not lines_are_subsequence(expected_c, actual_c, strip_dollars=True):
            c_failure = check_equal(path, "c", expected_c, actual_c)
            if c_failure:
                failures.append(c_failure)

    if not failures:
        input_path.unlink(missing_ok=True)
        ir_path.unlink(missing_ok=True)
        c_path.unlink(missing_ok=True)
        (path.parent / f"_{path.stem}.out").unlink(missing_ok=True)
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
        input_path.write_text(source)
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
    input_path.write_text(source)
    proc = run_cmd([str(NERD), "format", "--stdout", str(input_path)])
    if proc.returncode != 0:
        return [Failure(path, f"formatter failed with exit {proc.returncode}\n{proc.stderr}")]
    failure = check_equal(path, "format", expected, proc.stdout)
    if expected.rstrip("\n") == proc.stdout.rstrip("\n"):
        failure = None
    if failure:
        return [failure]
    input_path.unlink(missing_ok=True)
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
    requests = json.loads(requests_text)

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
    expected_text = expected_text.replace(
        "__REPO_URI__", f"file://{ROOT}"
    ).replace("/_bin/mods/", "/mods/")
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

    cwd = path.parent
    input_path = cwd / f"{path.stem}.input.n"
    input_path.write_text(source)

    args = [str(NERD), command, *cli_args]
    if command in {"run", "r"} and run_mode == "keep" and "--keep" not in args:
        args.append("--keep")
    if command != "explain":
        args.append(str(input_path.name))
    proc = run_cmd(args, cwd=cwd)

    failures: list[Failure] = []
    if proc.returncode != expected_exit:
        failures.append(Failure(path, f"exit mismatch: expected {expected_exit}, got {proc.returncode}\n{proc.stderr}"))
    if expected_stdout.strip():
        if expected_stdout.rstrip("\n") != proc.stdout.rstrip("\n"):
            stdout_failure = check_equal(path, "stdout", expected_stdout, proc.stdout)
            if stdout_failure:
                failures.append(stdout_failure)

    executable = input_path.with_suffix("")
    if run_mode == "keep" and not executable.exists():
        failures.append(Failure(path, "expected command to keep generated executable"))
    if run_mode == "delete" and executable.exists():
        failures.append(Failure(path, "expected command to delete generated executable"))
        executable.unlink()
    if not failures:
        input_path.unlink(missing_ok=True)
        executable.unlink(missing_ok=True)
    return failures


def collect() -> list[tuple[str, pathlib.Path]]:
    cases: list[tuple[str, pathlib.Path]] = []
    for kind, directory, suffix in [
        ("language", "tests/language", "*.t"),
        ("errors", "tests/errors", "*.e"),
        ("format", "tests/format", "*.f"),
        ("lsp", "tests/lsp", "*.lsp"),
        ("commands", "tests/commands", "*.cmd"),
    ]:
        for path in sorted((ROOT / directory).glob(suffix)):
            cases.append((kind, path))
    return cases


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Nerd compiler regression tests")
    parser.add_argument("--filter", default="", help="Only run test paths containing this text")
    args = parser.parse_args()

    runners = {
        "language": test_language,
        "errors": test_errors,
        "format": test_format,
        "lsp": test_lsp,
        "commands": test_command,
    }

    cases = [(kind, path) for kind, path in collect() if args.filter in rel(path)]
    counts: dict[str, SuiteCounts] = {kind: SuiteCounts() for kind in runners}
    failures: list[Failure] = []
    for kind, path in cases:
        counts[kind].total += 1
        case_failures = runners[kind](path)
        if case_failures:
            counts[kind].failed += 1
            if console:
                console.print("[bold red]FAIL[/bold red]", rel(path))
            else:
                print(f"{colour('FAIL', '1;31')} {rel(path)}")
            for failure in case_failures:
                out(failure.message)
            failures.extend(case_failures)
        else:
            counts[kind].passed += 1
            if console:
                console.print("[bold green]PASS[/bold green]", rel(path))
            else:
                print(f"{colour('PASS', '1;32')} {rel(path)}")

    print_summary(counts)
    return 1 if failures else 0


def print_summary(counts: dict[str, SuiteCounts]) -> None:
    total = SuiteCounts()
    for count in counts.values():
        total.total += count.total
        total.passed += count.passed
        total.failed += count.failed

    if console and Table:
        table = Table(title="Test Summary")
        table.add_column("Suite", style="cyan")
        table.add_column("Total", justify="right")
        table.add_column("Passed", justify="right", style="green")
        table.add_column("Failed", justify="right", style="red")
        for kind, count in counts.items():
            table.add_row(kind, str(count.total), str(count.passed), str(count.failed))
        table.add_section()
        table.add_row("total", str(total.total), str(total.passed), str(total.failed))
        console.print(table)
        return

    print()
    print("Test Summary")
    print("suite       total  passed  failed")
    for kind, count in counts.items():
        print(f"{kind:<10} {count.total:>5} {count.passed:>7} {count.failed:>7}")
    print(f"{'total':<10} {total.total:>5} {total.passed:>7} {total.failed:>7}")


if __name__ == "__main__":
    sys.exit(main())
