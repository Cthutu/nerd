from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from shutil import which

ROOT = Path(__file__).resolve().parent.parent

C_SEARCH_ROOTS = [
    ROOT / "src",
    ROOT / "tests",
]

NERD_SEARCH_ROOTS = [
    ROOT / "mods",
    ROOT / "examples",
]

SKIP_DIRS = {
    "_bin",
    "_obj",
    ".git",
    "node_modules",
    "out",
}

C_SUFFIXES = {".c", ".h"}
NERD_SUFFIXES = {".n"}


def iter_source_files(search_roots: list[Path], suffixes: set[str]) -> list[Path]:
    files: list[Path] = []
    for search_root in search_roots:
        if not search_root.exists():
            continue

        for path in search_root.rglob("*"):
            if not path.is_file():
                continue
            if path.suffix not in suffixes:
                continue
            if any(part in SKIP_DIRS for part in path.parts):
                continue
            files.append(path)

    return sorted(set(files))


def nerd_executable() -> str | None:
    for candidate in [
        ROOT
        / "_bin"
        / ("nerd-debug.exe" if sys.platform == "win32" else "nerd-debug"),
        ROOT / "_bin" / ("nerd.exe" if sys.platform == "win32" else "nerd"),
    ]:
        if candidate.exists():
            return str(candidate)
    return which("nerd")


def main() -> int:
    c_files = iter_source_files(C_SEARCH_ROOTS, C_SUFFIXES)
    nerd_files = iter_source_files(NERD_SEARCH_ROOTS, NERD_SUFFIXES)

    if c_files:
        result = subprocess.run(
            ["clang-format", "-i", *[str(path) for path in c_files]]
        )
        if result.returncode != 0:
            return result.returncode

    if nerd_files:
        nerd = nerd_executable()
        if nerd is None:
            print(
                "Could not find nerd formatter. Build nerd or install it first.",
                file=sys.stderr,
            )
            return 1
        for path in nerd_files:
            result = subprocess.run([nerd, "format", "-v", str(path)])
            if result.returncode != 0:
                return result.returncode

    if not c_files and not nerd_files:
        print("No files to format.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
