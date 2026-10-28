"""Build a distribution archive from this checkout's release outputs."""

import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile
from zipfile import ZIP_DEFLATED, ZipFile


ROOT = Path(__file__).resolve().parents[1]


def package(root: Path, output: Path) -> None:
    manifest = json.loads((root / "syntax/nerd-vscode/package.json").read_text(encoding="utf-8"))
    extension = f"{manifest['name']}-{manifest['version']}.vsix"
    compiler = "nerd.exe" if os.name == "nt" else "nerd"
    entries = [(root / "_bin" / compiler, compiler),
               (root / "syntax/nerd-vscode" / extension, extension),
               (root / "etc/llvm-tools.json", "llvm-tools.json")]
    entries.extend((root / name, name) for name in ["README.md", "DEBUGGER.md", "ROADMAP.md"])
    tracked = subprocess.check_output(
        ["git", "ls-files", "-z", "--", "mods", "examples", "docs"], cwd=root,
    ).decode("utf-8").split("\0")
    entries.extend((root / name, name) for name in tracked if name)
    for source, _ in entries:
        if not source.is_file():
            raise FileNotFoundError(f"Missing distribution input: {source}")
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=output.parent, prefix="nerd-", suffix=".zip", delete=False) as stream:
        temporary = Path(stream.name)
    try:
        with ZipFile(temporary, "w", ZIP_DEFLATED) as archive:
            for source, name in entries:
                archive.write(source, name)
        temporary.replace(output)
    finally:
        temporary.unlink(missing_ok=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-build", action="store_true")
    parser.add_argument("--output", type=Path, default=ROOT / "_dist/nerd.zip")
    args = parser.parse_args()
    if not args.skip_build:
        subprocess.run(["just", "build-release", "nerd", "--skip-mod-sync"], cwd=ROOT, check=True)
        subprocess.run(["just", "package"], cwd=ROOT, check=True)
    package(ROOT, args.output.resolve())
    print(f"Created {args.output}")


if __name__ == "__main__":
    main()
