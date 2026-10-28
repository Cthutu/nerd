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
                "docs/learn/example.md": "[Chapter](README.md)\n[Toolchain](../toolchain.md#setup)\n```nerd\n-- [Code](../toolchain.md)\n```\n",
                "docs/toolchain.md": "developer documentation",
                "README.md": "repository guide",
                "DEBUGGER.md": "debugger guide",
                "ROADMAP.md": "open work",
            }
            for name, content in files.items():
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(content, encoding="utf-8")
            subprocess.run(["git", "add", "mods", "examples", "docs"], cwd=root, check=True)
            hooks = root / "disabled-hooks"
            hooks.mkdir()
            subprocess.run(["git", "-c", "user.name=Distribution Test", "-c", "user.email=test@example.invalid",
                            "-c", f"core.hooksPath={hooks}", "-c", "commit.gpgsign=false",
                            "commit", "-qm", "fixture"], cwd=root, check=True)
            revision = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
            (root / "examples/hello/main.out").write_bytes(b"untracked binary")
            output = root / "_dist/nerd.zip"
            package(root, output)
            with ZipFile(output) as archive:
                self.assertEqual(set(archive.namelist()), {
                    "nerd.exe" if os.name == "nt" else "nerd",
                    "llvm-tools.json", "mods/core.n", "examples/hello/main.n", "docs/learn/README.md",
                    "docs/learn/example.md",
                })
                self.assertEqual(archive.read("docs/learn/README.md"), b"learning guide")
                self.assertEqual(archive.read("docs/learn/example.md").decode(),
                                 '[Chapter](README.md)\n'
                                 f'[Toolchain](https://github.com/Cthutu/nerd/blob/{revision}/docs/toolchain.md#setup)\n'
                                 '```nerd\n-- [Code](../toolchain.md)\n```\n')
            previous = output.read_bytes()
            (root / "mods/core.n").unlink()
            with self.assertRaises(FileNotFoundError):
                package(root, output)
            self.assertEqual(output.read_bytes(), previous)


if __name__ == "__main__":
    unittest.main()
