#!/usr/bin/env python3
"""Exercise shared allocator bookkeeping independently of compiler scheduling."""
import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--sanitize', choices=['address', 'thread'])
    args = parser.parse_args()
    cc = shlex.split(os.environ.get('CC', 'clang'))
    flags = ['-std=c23', '-D_GNU_SOURCE', '-I' + str(ROOT / 'src'), '-g', '-O1']
    if os.name != 'nt':
        flags += ['-pthread']
    if args.sanitize:
        flags += ['-fsanitize=' + args.sanitize, '-fno-omit-frame-pointer']
    with tempfile.TemporaryDirectory(prefix='nerd-memory-') as directory:
        for release in [False, True]:
            output = Path(directory) / ('memory.exe' if os.name == 'nt' else 'memory')
            command = cc + flags + (['-DNDEBUG'] if release else []) + [
                str(ROOT / 'tests/core/memory-concurrency.c'),
                str(ROOT / 'src/core/memory.c'), '-o', str(output)]
            subprocess.run(command, check=True)
            for _ in range(3):
                result = subprocess.run([str(output)], capture_output=True,
                                        text=True, timeout=60)
                assert result.returncode == 0, result.stderr
                assert result.stdout == 'memory-concurrency ok\n', result
                if not release:
                    assert '1 leaks, 7 bytes' in result.stderr, result.stderr
    print('Concurrent allocator counters, cross-thread ownership and debug tracking passed')


if __name__ == '__main__':
    main()
