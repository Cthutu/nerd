#!/usr/bin/env python3
"""Check runtime-owned primitives and the first Linux libc-removal boundary."""
import os
from pathlib import Path
import platform
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

# Deliberately not a claim that the complete runtime is freestanding yet.
REMOVED_SYMBOLS = {
    'bcmp', 'memcmp', 'memcpy', 'memmove', 'strcmp', 'strlen', 'calloc', 'free',
    'mmap', 'mprotect', 'munmap',
}


def main():
    cc = shlex.split(os.environ.get('CC', 'clang'))
    raw_linux = platform.system() == 'Linux' and platform.machine() in {
        'x86_64', 'aarch64', 'arm64',
    }
    with tempfile.TemporaryDirectory(prefix='nerd-runtime-foundation-') as directory:
        directory = Path(directory)
        output = directory / ('runtime.exe' if os.name == 'nt' else 'runtime')
        for release in (False, True):
            flags = ['-std=c23', '-Wall', '-Wextra', '-Werror']
            flags += ['-DNDEBUG'] if release else []
            flags += ['-D_GNU_SOURCE'] if os.name != 'nt' else []
            for optimisation in ('-O0', '-O2'):
                subprocess.run(cc + flags + [optimisation,
                    str(ROOT / 'tests/core/runtime-foundation.c'), '-o', str(output)],
                    check=True, timeout=60)
                result = subprocess.run([str(output)], capture_output=True,
                    text=True, encoding='utf-8', timeout=30)
                assert result.returncode == 0, result.stderr
                assert result.stdout == 'runtime foundation passed\n', result.stdout
                assert result.stderr == '', result.stderr
            if raw_linux:
                # Match the optimised embedded runtime build, including PIC.
                # Inspect nrt alone: the host test oracle intentionally uses libc.
                for pic in (False, True):
                    obj = directory / 'nrt.o'
                    subprocess.run(cc + flags + ['-O2'] + (['-fPIC'] if pic else []) + [
                        '-c', str(ROOT / 'data/nrt.c'), '-o', str(obj)],
                        check=True, timeout=60)
                    symbols = subprocess.check_output(['nm', '-u', str(obj)], text=True)
                    undefined = {line.split()[-1].split('@')[0]
                                 for line in symbols.splitlines() if line.split()}
                    assert not undefined & REMOVED_SYMBOLS, symbols
            print('[PASS] runtime foundation:', 'release' if release else 'debug')


if __name__ == '__main__':
    main()
