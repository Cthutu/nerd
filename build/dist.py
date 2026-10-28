"""Build a distribution archive from this checkout's release outputs."""

import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile
from zipfile import ZIP_DEFLATED, ZipFile


ROOT = Path(__file__).resolve().parents[1]


def package(root: Path, output: Path) -> None:
    compiler = "nerd.exe" if os.name == "nt" else "nerd"
    entries = [(root / "_bin" / compiler, compiler),
               (root / "etc/llvm-tools.json", "llvm-tools.json")]
    tracked = subprocess.check_output(
        ["git", "ls-files", "-z", "--", "mods", "examples", "docs/learn"], cwd=root,
    ).decode("utf-8").split("\0")
    entries.extend((root / name, name) for name in tracked if name)
    for directory in ["mods", "examples", "docs/learn"]:
        if not any(name.startswith(directory + "/") for _, name in entries):
            raise FileNotFoundError(f"Missing distribution directory contents: {directory}")
    for source, _ in entries:
        if not source.is_file():
            raise FileNotFoundError(f"Missing distribution input: {source}")
    revision = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=output.parent, prefix="nerd-", suffix=".zip", delete=False) as stream:
        temporary = Path(stream.name)
    try:
        with ZipFile(temporary, "w", ZIP_DEFLATED) as archive:
            for source, name in entries:
                if name.startswith("docs/learn/") and source.suffix == ".md":
                    text = source.read_text(encoding="utf-8")

                    def link(match: re.Match) -> str:
                        target = match[2]
                        path, _, fragment = target.partition("#")
                        if not path or ":" in path or not path.endswith(".md"):
                            return match[0]
                        destination = (source.parent / path).resolve()
                        if destination.is_relative_to((root / "docs/learn").resolve()):
                            return match[0]
                        if not destination.is_relative_to(root.resolve()):
                            return match[0]
                        relative = destination.relative_to(root.resolve()).as_posix()
                        url = f"https://github.com/Cthutu/nerd/blob/{revision}/{relative}"
                        if fragment:
                            url += "#" + fragment
                        return match[1] + url + match[3]

                    # Keep code fences verbatim; only prose links to omitted
                    # developer/reference docs need repository URLs.
                    parts = re.split(r"(^```.*?^```[^\n]*$)", text, flags=re.M | re.S)
                    for index in range(0, len(parts), 2):
                        parts[index] = re.sub(r"(\[[^\]\n]*\]\()([^\n)]+)(\))", link, parts[index])
                    archive.writestr(name, "".join(parts))
                else:
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
    package(ROOT, args.output.resolve())
    print(f"Created {args.output}")


if __name__ == "__main__":
    main()
