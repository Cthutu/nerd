#!/usr/bin/env python3
"""Check long LLVM arithmetic trees, operand order and serial/worker parity."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug')
    parser.add_argument('--terms', type=int, default=6000,
                        help='Number of operands in the left-deep arithmetic regression')
    args = parser.parse_args()
    if args.terms < 2:
        parser.error('--terms must be at least two')
    nerd = args.nerd.resolve()
    with tempfile.TemporaryDirectory(prefix='nerd-expression-depth-') as directory:
        work = Path(directory)
        count = args.terms
        left = ' - '.join(f'next({i})' for i in range(1, count + 1))
        right = 'next(64)'
        right_value = 64
        for value in range(63, 0, -1):
            right = f'next({value}) - (' + right + ')'
            right_value = value - right_value
        (work / 'deep.n').write_text(
            'counter: i32 = 0\nordered: bool = true\n'
            'next :: fn (expected: i32) -> i32 { counter += 1\n'
            'on counter != expected => ordered = false\n return counter }\n'
            'pub check :: fn () -> i32 {\n'
            'counter = 0\n value := ' + left + '\n'
            f'on value != {2 - count * (count + 1) // 2} => return 1\n'
            f'on counter != {count} => return 2\n'
            'counter = 0\n other := ' + right + '\n'
            f'on other != {right_value} => return 3\n'
            'on counter != 64 => return 4\n on !ordered => return 5\n return 0\n}\n')
        (work / 'sibling.n').write_text('pub check :: fn () -> i32 { return 0 }\n')
        (work / 'main.n').write_text('deep :: use deep\nsibling :: use sibling\n'
                                    'main :: fn () -> i32 { return deep.check() + sibling.check() }\n')
        binary = work / ('program.exe' if os.name == 'nt' else 'program')
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'),
                   NERD_DEBUG_KEEP_LINK_LLVM='1')
        reference = None
        for jobs in ['1', '4']:
            result = subprocess.run([str(nerd), 'build', '--jobs', jobs,
                                     '-o', str(binary), str(work / 'main.n')],
                                    env=env, capture_output=True, text=True, timeout=180)
            assert result.returncode == 0, (jobs, result.stdout, result.stderr)
            ir = Path(str(binary) + '.link.ll').read_bytes()
            if reference is None:
                reference = ir
            assert ir == reference, 'Serial and worker LLVM differ'
            subprocess.run([str(binary)], check=True, timeout=10)
            print(f'[PASS] {count}-term arithmetic, right nesting and operand order: jobs={jobs}', flush=True)


if __name__ == '__main__':
    main()
