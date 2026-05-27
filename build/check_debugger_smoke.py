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

Choice :: enum {
    None
    Some(i32)
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
    choice := Choice.Some(7)
    values := [10, 20, 30]
    bytes := [1.as(u8), 2.as(u8), 3.as(u8)]
    slice := bytes[..]
    numbers: [..]i32
    numbers.push(10)
    numbers.push(20)
    on choice == Choice.Some(7) && point.x == 3 && values[1] == 20 && bytes[2] == 3 &&
       slice.data[1] == 2 && numbers[1] == 20 => prn(message)
}
"""


def run(cmd: list[str], *, cwd: pathlib.Path = ROOT) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)


def latest_codelldb_lldb() -> pathlib.Path | None:
    candidates: list[pathlib.Path] = []
    home = pathlib.Path.home()
    extension_bases = [
        home / ".vscode" / "extensions",
        home / ".vscode-insiders" / "extensions",
        home / "scoop" / "persist" / "vscode" / "data" / "extensions",
        home / "scoop" / "persist" / "vscode-insiders" / "data" / "extensions",
    ]
    for base in extension_bases:
        if not base.exists():
            continue
        candidates.extend(base.glob("vadimcn.vscode-lldb-*/lldb/bin/lldb"))
        candidates.extend(base.glob("vadimcn.vscode-lldb-*/lldb/bin/lldb.exe"))

    def version_key(path: pathlib.Path) -> tuple[int, ...]:
        import re

        match = re.search(r"vscode-lldb-(\d+(?:\.\d+)*)", str(path))
        if match is None:
            return ()
        return tuple(int(part) for part in match.group(1).split("."))

    return sorted(candidates, key=version_key)[-1] if candidates else None


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
        return fail("CodeLLDB-bundled LLDB was not found")

    exe_suffix = ".exe" if sys.platform == "win32" else ""
    work = ROOT / "_tmp" / "debugger-smoke"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)

    source = work / "main.n"
    binary = work / f"main{exe_suffix}"
    source_for_lldb = source.as_posix()
    source.write_text(SMOKE_SOURCE, encoding="utf-8", newline="\n")

    build = run([str(nerd), "build", "--output", str(binary), str(source)])
    if build.returncode != 0:
        return fail("Nerd debug build failed", build.stderr)

    commands = [
        "target create " + str(binary),
        "breakpoint set --file " + source_for_lldb + " --line 15",
        "breakpoint set --file " + source_for_lldb + " --line 30",
        "run",
        "bt",
        "frame variable min_width min_height",
        "expression min_width",
        "expression min_height",
        "continue",
        "target variable saved_width saved_height",
        "frame variable message point choice values bytes slice numbers",
        "expression saved_width",
        "expression saved_height",
        "expression point.x",
        "expression message.count",
        "expression message.data[0]",
        "expression values[1]",
        "expression bytes[2]",
        "expression slice.data[1]",
        "expression numbers[1]",
        "expression numbers ? *((unsigned long long *)((char *)numbers - 16)) : 0",
        "expression numbers ? *((unsigned long long *)((char *)numbers - 8)) : 0",
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
        "`set_default_size(min_width=41, min_height=19) at main.n:15:1",
        "`main at main.n:20:1",
        "(unsigned int) min_width = 41",
        "(unsigned int) min_height = 19",
        "(unsigned int) saved_width = 41",
        "(unsigned int) saved_height = 19",
        '(string) message = (data = "hello", count = 5)',
        "(Point) point = {",
        "x = 3",
        "y = 4",
        "(Choice) choice = (tag = ",
        "payload = 7",
        "(int[3]) values = ([0] = 10, [1] = 20, [2] = 30)",
        '(unsigned char[3]) bytes = "\\U00000001\\U00000002\\U00000003"',
        "([]u8) slice = (data = ",
        "count = 3)",
        "(int *) numbers = ",
        "(unsigned int) $0 = 41",
        "(unsigned int) $1 = 19",
        "(unsigned int) $2 = 41",
        "(unsigned int) $3 = 19",
        "(int) $4 = 3",
        "$5 = 5",
        "(unsigned char) $6 = 'h'",
        "(int) $7 = 20",
        "(unsigned char) $8 = '\\x03'",
        "(unsigned char) $9 = '\\x02'",
        "(int) $10 = 20",
        "(unsigned long long) $11 = 2",
        "(unsigned long long) $12 = 2",
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
