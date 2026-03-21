from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

SEARCH_ROOTS = [
    ROOT / "src",
    ROOT / "tests",
]

SKIP_DIRS = {
    "_bin",
    "_obj",
    ".git",
    "node_modules",
    "out",
}

SUFFIXES = {".c", ".h"}


def iter_source_files() -> list[Path]:
    files: list[Path] = []
    for search_root in SEARCH_ROOTS:
        if not search_root.exists():
            continue

        for path in search_root.rglob("*"):
            if not path.is_file():
                continue
            if path.suffix not in SUFFIXES:
                continue
            if any(part in SKIP_DIRS for part in path.parts):
                continue
            files.append(path)

    return sorted(set(files))


def main() -> int:
    files = iter_source_files()
    if not files:
        print("No files to format.")
        return 0

    cmd = ["clang-format", "-i", *[str(path) for path in files]]
    result = subprocess.run(cmd)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
