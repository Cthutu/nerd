#!/usr/bin/env python3
import argparse
import pathlib
import re
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]


SOURCE = """mix :: fn (left: i32, right: i32) -> i32 {
    total := left + right
    return total
}

main :: fn () {
    first: i32
    first = 10
    second: i32
    second = first + 1
    third: i32
    third = mix(second, 3)
    on third == 14 => prn("ok")
}
"""


def run(cmd: list[str], *, cwd: pathlib.Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)


def latest_codelldb_lldb() -> pathlib.Path | None:
    candidates: list[pathlib.Path] = []
    for base in [pathlib.Path.home() / ".vscode" / "extensions"]:
        if base.exists():
            candidates.extend(base.glob("vadimcn.vscode-lldb-*/lldb/bin/lldb"))
    return sorted(candidates)[-1] if candidates else None


def fail(message: str, details: str = "") -> int:
    print(f"debugger-stepping failed: {message}", file=sys.stderr)
    if details:
        print(details, file=sys.stderr)
    return 1


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run a Linux LLDB stepping smoke test for Nerd debug info"
    )
    parser.add_argument(
        "--nerd",
        default=str(ROOT / "_bin" / "nerd-debug"),
        help="Nerd compiler executable to use",
    )
    parser.add_argument(
        "--lldb",
        default=None,
        help="LLDB executable to use; defaults to the latest CodeLLDB-bundled LLDB",
    )
    args = parser.parse_args()

    nerd = pathlib.Path(args.nerd)
    if not nerd.exists():
        return fail(f"Nerd compiler does not exist: {nerd}")

    lldb = pathlib.Path(args.lldb) if args.lldb else latest_codelldb_lldb()
    if lldb is None or not lldb.exists():
        return fail("CodeLLDB-bundled LLDB was not found under ~/.vscode/extensions")

    work = ROOT / "_tmp" / "debugger-stepping"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)

    source = work / "main.n"
    binary = work / "main"
    source.write_text(SOURCE, encoding="utf-8", newline="\n")

    build = run([str(nerd), "build", "--output", str(binary), str(source)])
    if build.returncode != 0:
        return fail("Nerd debug build failed", build.stderr)

    source_step_commands = [
        "target create " + str(binary),
        "breakpoint set --file main.n --line 8",
        "run",
        "frame info",
        "thread step-over",
        "frame info",
        "thread step-over",
        "frame info",
        "thread step-over",
        "frame info",
    ]

    lldb_cmd = [str(lldb), "--batch"]
    for command in source_step_commands:
        lldb_cmd.extend(["-o", command])

    debug = run(lldb_cmd)
    output = debug.stdout + debug.stderr
    if debug.returncode != 0:
        return fail("LLDB stepping session failed", output)

    raw_frame_lines = [
        int(match.group(1))
        for match in re.finditer(r"frame #0: .* at main\.n:(\d+):", output)
    ]
    frame_lines: list[int] = []
    for line in raw_frame_lines:
        if not frame_lines or frame_lines[-1] != line:
            frame_lines.append(line)
    if frame_lines[:4] != [8, 9, 10, 11]:
        return fail(
            "LLDB step-over did not visit the expected Nerd source lines",
            f"lines: {frame_lines}\n\noutput:\n{output}",
        )

    call_stack_commands = [
        "target create " + str(binary),
        "breakpoint set --file main.n --line 12",
        "run",
        "thread step-in",
        "bt",
        "frame variable left right total",
        "thread step-over",
        "bt",
        "frame variable left right total",
        "thread step-out",
        "bt",
        "thread step-over",
        "frame variable third",
    ]
    lldb_cmd = [str(lldb), "--batch"]
    for command in call_stack_commands:
        lldb_cmd.extend(["-o", command])

    debug = run(lldb_cmd)
    output = debug.stdout + debug.stderr
    if debug.returncode != 0:
        return fail("LLDB call-stack stepping session failed", output)

    expected = [
        "stop reason = step in",
        "`mix(left=11, right=3) at main.n:2:1",
        "`main at main.n:12:1",
        "(int) left = 11",
        "(int) right = 3",
        "(int) total = 14",
        "`mix(left=11, right=3) at main.n:3:1",
        "stop reason = step out",
        "`main at main.n:12:1",
        "(int) third = 14",
    ]
    missing = [item for item in expected if item not in output]
    if missing:
        return fail(
            "LLDB call-stack stepping output did not contain expected evidence",
            "missing:\n"
            + "\n".join(missing)
            + "\n\noutput:\n"
            + output,
        )

    print("debugger-stepping ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
