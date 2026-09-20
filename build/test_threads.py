#!/usr/bin/env python3
"""Exercise portable thread and condition-variable lifecycles."""
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
    with tempfile.TemporaryDirectory(prefix='nerd-threads-') as directory:
        for release, injected in [(False, False), (True, False),
                                  (False, True), (True, True)]:
            output = Path(directory) / ('threads.exe' if os.name == 'nt' else 'threads')
            implementation = ('tests/core/thread-create-failure.c' if injected
                              else 'src/core/thread.c')
            defines = (['-DNDEBUG'] if release else [])
            if injected:
                defines += ['-DNERD_TEST_START_FAILURE=1']
            command = cc + flags + defines + [
                str(ROOT / 'tests/core/thread-lifecycle.c'),
                str(ROOT / 'src/core/mutex.c'),
                str(ROOT / 'src/core/task.c'),
                str(ROOT / implementation), '-o', str(output)]
            subprocess.run(command, check=True)
            for _ in range(3):
                result = subprocess.run([str(output)], capture_output=True,
                                        text=True, timeout=60)
                assert result.returncode == 0, result.stderr
                assert result.stdout == 'thread-lifecycle ok\n', result
    print('Thread startup, joins, predicate wakeups and partial-start cleanup passed')


if __name__ == '__main__':
    main()
