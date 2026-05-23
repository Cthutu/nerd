#!/usr/bin/env python3
import argparse
import pathlib
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]


SMOKE_SOURCE = """Point :: plex {
    x i32
    y i32
}

saved_width: u32
saved_height: u32

set_default_size :: fn (min_width: u32 = 41, min_height: u32 = 19) {
    saved_width = min_width
    saved_height = min_height
}

main :: fn () {
    set_default_size()
    message := "hello"
    point := Point { x: 3 y: 4 }
    values := [10, 20, 30]
    bytes := [1.as(u8), 2.as(u8), 3.as(u8)]
    slice := bytes[..]
    numbers: [..]i32
    numbers.push(10)
    numbers.push(20)
    on point.x == 3 && values[1] == 20 && bytes[2] == 3 &&
       slice.data[1] == 2 && numbers[1] == 20 => prn(message)
}
"""


def run(cmd: list[str], *, cwd: pathlib.Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)


def latest_codelldb_lldb() -> pathlib.Path | None:
    candidates: list[pathlib.Path] = []
    for base in [pathlib.Path.home() / ".vscode" / "extensions"]:
        if not base.exists():
            continue
        candidates.extend(base.glob("vadimcn.vscode-lldb-*/lldb/bin/lldb"))
    return sorted(candidates)[-1] if candidates else None


def fail(message: str, details: str = "") -> int:
    print(f"debugger-smoke failed: {message}", file=sys.stderr)
    if details:
        print(details, file=sys.stderr)
    return 1


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run a Linux CodeLLDB/LLDB smoke test for Nerd debug info"
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

    work = ROOT / "_tmp" / "debugger-smoke"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)

    source = work / "main.n"
    binary = work / "main"
    source.write_text(SMOKE_SOURCE, encoding="utf-8", newline="\n")

    build = run([str(nerd), "build", "--output", str(binary), str(source)])
    if build.returncode != 0:
        return fail("Nerd debug build failed", build.stderr)

    commands = [
        "target create " + str(binary),
        "breakpoint set --file main.n --line 9",
        "breakpoint set --file main.n --line 24",
        "run",
        "frame variable min_width min_height",
        "expression min_width",
        "expression min_height",
        "continue",
        "frame variable message point values bytes slice numbers",
        "expression point.x",
        "expression message.data[0]",
        "expression values[1]",
        "expression bytes[2]",
        "expression slice.data[1]",
        "expression numbers[1]",
    ]
    lldb_cmd = [str(lldb), "--batch"]
    for command in commands:
        lldb_cmd.extend(["-o", command])

    debug = run(lldb_cmd)
    output = debug.stdout + debug.stderr
    if debug.returncode != 0:
        return fail("LLDB smoke session failed", output)

    expected = [
        "Breakpoint 1:",
        "Breakpoint 2:",
        "stop reason = breakpoint",
        "(unsigned int) min_width = 41",
        "(unsigned int) min_height = 19",
        '(string) message = (data = "hello", count = 5)',
        "(Point) point = {",
        "x = 3",
        "y = 4",
        "(int[3]) values = ([0] = 10, [1] = 20, [2] = 30)",
        '(unsigned char[3]) bytes = "\\U00000001\\U00000002\\U00000003"',
        "([]u8) slice = (data = ",
        "count = 3)",
        "(int *) numbers = ",
        "(unsigned int) $0 = 41",
        "(unsigned int) $1 = 19",
        "(int) $2 = 3",
        "(unsigned char) $3 = 'h'",
        "(int) $4 = 20",
        "(unsigned char) $5 = '\\x03'",
        "(unsigned char) $6 = '\\x02'",
        "(int) $7 = 20",
    ]
    missing = [item for item in expected if item not in output]
    if missing:
        return fail(
            "LLDB smoke output did not contain expected debugger evidence",
            "missing:\n"
            + "\n".join(missing)
            + "\n\noutput:\n"
            + output,
        )

    print("debugger-smoke ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
