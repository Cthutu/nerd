"""Package Nerd's release compiler, VS Code extension, modules, and tutorials."""

from pathlib import Path
import json
import subprocess
import tempfile
from zipfile import ZIP_DEFLATED, ZipFile


ROOT = Path(__file__).resolve().parents[1]
NERD_ROOT = Path.home() / "nerd"
OUTPUT = ROOT / "_dist" / "nerd.zip"


def add_directory(archive: ZipFile, source: Path, name: str) -> None:
    archive.write(source, name + "/")
    for path in sorted(source.rglob("*")):
        archive.write(path, (Path(name) / path.relative_to(source)).as_posix())


def main() -> None:
    directories = {
        "mods": NERD_ROOT / "mods",
        "examples": NERD_ROOT / "examples",
        "docs": ROOT / "docs",
    }
    for source in directories.values():
        if not source.is_dir():
            raise SystemExit(f"Missing package directory: {source}")

    subprocess.run(
        ["just", "build-release", "nerd", "--skip-mod-sync"],
        cwd=NERD_ROOT,
        check=True,
    )
    compiler = NERD_ROOT / "_bin" / "nerd.exe"
    if not compiler.is_file():
        raise SystemExit(f"Release compiler not found: {compiler}")

    subprocess.run(["just", "package"], cwd=NERD_ROOT, check=True)
    extension_dir = NERD_ROOT / "syntax" / "nerd-vscode"
    manifest = json.loads((extension_dir / "package.json").read_text(encoding="utf-8"))
    vsix = extension_dir / f"{manifest['name']}-{manifest['version']}.vsix"
    if not vsix.is_file():
        raise SystemExit(f"VS Code extension not found: {vsix}")

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    # Finish the new archive before replacing a previous package.
    with tempfile.NamedTemporaryFile(
        dir=OUTPUT.parent, prefix="nerd-", suffix=".zip", delete=False
    ) as temporary:
        temporary_path = Path(temporary.name)
    try:
        with ZipFile(temporary_path, "w", ZIP_DEFLATED) as archive:
            archive.write(compiler, "nerd.exe")
            archive.write(vsix, vsix.name)
            for name, source in directories.items():
                add_directory(archive, source, name)
        temporary_path.replace(OUTPUT)
    finally:
        temporary_path.unlink(missing_ok=True)

    print(f"Created {OUTPUT}")


if __name__ == "__main__":
    main()
