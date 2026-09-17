#!/usr/bin/env python3
"""Check the real clean recipe in an isolated copy of the workspace layout."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    with tempfile.TemporaryDirectory(prefix="nerd-clean-") as directory:
        work = Path(directory)
        (work / "Justfile").write_bytes((ROOT / "Justfile").read_bytes())
        sources = {
            "tests/ffi/variadic_host.c": (ROOT / "tests/ffi/variadic_host.c").read_bytes(),
            "tests/commands/source.c": b"/* C source fixture */\n",
            "tests/mods/source.n": b"value :: 42\n",
        }
        artifacts = (
            "_bin/nerd-debug", "_tmp/generated.c",
            "tests/commands/example.host.c", "tests/commands/example.input.c",
            "tests/commands/example.input.n", "tests/commands/example.out",
            "tests/commands/_example.hir", "tests/commands/_example.ll",
            "tests/commands/example.m1.ll", "tests/commands/example.link.ll",
            "tests/format/example.format", "tests/lsp/example.lsp.in",
            "tests/lsp/example.lsp.out",
        )
        for name, content in {**sources, **dict.fromkeys(artifacts, b"generated\n")}.items():
            path = work / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(content)
        subprocess.run(["just", "clean"], cwd=work, check=True, capture_output=True)
        for name, content in sources.items():
            assert (work / name).read_bytes() == content, f"clean changed source fixture: {name}"
        for name in artifacts:
            assert not (work / name).exists(), f"clean left generated artifact: {name}"
    print("Clean recipe preserves source fixtures and removes generated artifacts")


if __name__ == "__main__":
    main()
