"""Check distribution contents and safe archive replacement without builds."""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from zipfile import ZipFile

from dist import package


class DistributionTests(unittest.TestCase):
    def test_contents_and_failed_replacement(self):
        with tempfile.TemporaryDirectory(prefix="nerd-dist-test-") as temporary:
            root = Path(temporary)
            subprocess.run(["git", "init", "-q", str(root)], check=True)
            files = {
                "syntax/nerd-vscode/package.json": json.dumps({"name": "test", "version": "1"}),
                "syntax/nerd-vscode/test-1.vsix": "extension",
                "_bin/nerd.exe" if os.name == "nt" else "_bin/nerd": "compiler",
                "etc/llvm-tools.json": "{}",
                "mods/core.n": "core",
                "examples/hello/main.n": "example",
                "docs/learn/README.md": "learning guide",
                "README.md": "repository guide",
                "DEBUGGER.md": "debugger guide",
                "ROADMAP.md": "open work",
            }
            for name, content in files.items():
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(content, encoding="utf-8")
            subprocess.run(["git", "add", "mods", "examples", "docs"], cwd=root, check=True)
            (root / "examples/hello/main.out").write_bytes(b"untracked binary")
            output = root / "_dist/nerd.zip"
            package(root, output)
            with ZipFile(output) as archive:
                self.assertEqual(set(archive.namelist()), {
                    "nerd.exe" if os.name == "nt" else "nerd", "test-1.vsix",
                    "llvm-tools.json", "mods/core.n", "examples/hello/main.n", "docs/learn/README.md",
                    "README.md", "DEBUGGER.md", "ROADMAP.md",
                })
                self.assertEqual(archive.read("docs/learn/README.md"), b"learning guide")
            previous = output.read_bytes()
            (root / "mods/core.n").unlink()
            with self.assertRaises(FileNotFoundError):
                package(root, output)
            self.assertEqual(output.read_bytes(), previous)


if __name__ == "__main__":
    unittest.main()
