#!/usr/bin/env python3
"""Exercise repository formatting with stale binaries and a failed rebuild."""
import argparse
import pathlib
import subprocess
import sys
import tempfile
from unittest.mock import patch

import format as workflow


SOURCE = 'build { on "windows" { library_path: $VULKAN_SDK/Lib } }\n'
EXPECTED = '''build {
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
}
'''


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--nerd', required=True)
    nerd = pathlib.Path(parser.parse_args().nerd).resolve()
    suffix = '.exe' if sys.platform == 'win32' else ''
    with tempfile.TemporaryDirectory(prefix='nerd-format-workflow-') as directory:
        root = pathlib.Path(directory)
        for name in ['build', '_bin', 'mods', 'src']:
            (root / name).mkdir()
        source = root / 'mods' / 'vulkan.n'
        source.write_text(SOURCE, encoding='utf-8')
        # Neither the stale debug binary nor the stale release binary may run.
        for name in ['nerd-debug', 'nerd']:
            (root / '_bin' / (name + suffix)).write_text('stale compiler')
        builder = root / 'build' / 'build.py'
        builder.write_text(
            'import pathlib, shutil, sys\n'
            'assert sys.argv[1:] == ["-r", "nerd", "--skip-mod-sync"]\n'
            f'shutil.copy2({str(nerd)!r}, {str(root / "_bin" / ("nerd" + suffix))!r})\n',
            encoding='utf-8',
        )
        with patch.multiple(workflow, ROOT=root, C_SEARCH_ROOTS=[root / 'src'],
                            NERD_SEARCH_ROOTS=[root / 'mods']):
            assert workflow.main() == 0
            assert source.read_text(encoding='utf-8') == EXPECTED
            assert workflow.main() == 0
            assert source.read_text(encoding='utf-8') == EXPECTED

            # A failed rebuild must leave both C and Nerd sources untouched.
            builder.write_text('raise SystemExit(17)\n', encoding='utf-8')
            source.write_text(SOURCE, encoding='utf-8')
            c_source = root / 'src' / 'sample.c'
            c_text = 'int    main( ){return 0;}\n'
            c_source.write_text(c_text, encoding='utf-8')
            assert workflow.main() == 17
            assert source.read_text(encoding='utf-8') == SOURCE
            assert c_source.read_text(encoding='utf-8') == c_text
    with tempfile.TemporaryDirectory(prefix='nerd-format-string-bytes-') as directory:
        source = pathlib.Path(directory) / 'main.n'
        source.write_text(r'''main :: fn () {
    escaped := "a\nb"
    physical := """a
b"""
    terminated := "a\nb\n"
    assert escaped.count == 3 && escaped[2] == 'b'
    assert physical == escaped
    assert terminated.count == 4 && terminated[3] == '\n'
}
''', encoding='utf-8')
        subprocess.run([str(nerd), 'format', str(source)], check=True)
        formatted = source.read_bytes()
        subprocess.run([str(nerd), 'format', str(source)], check=True)
        assert source.read_bytes() == formatted, 'String formatting must be idempotent'
        subprocess.run([str(nerd), 'run', str(source)], check=True)
    print('Repository formatting workflow and string-byte checks passed')


if __name__ == '__main__':
    main()
