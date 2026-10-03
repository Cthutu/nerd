#!/usr/bin/env python3
"""Run bounded, single-thread M0 language probes against explicit checkout modules."""
import argparse
import os
import shlex
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def run(command, env):
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                            text=True, encoding='utf-8', timeout=60)
    if result.returncode:
        raise SystemExit(f"Failed ({result.returncode}): {command}\n"
                         f"{result.stdout}{result.stderr}")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', required=True, type=Path)
    parser.add_argument('--cgen', action='store_true',
                        help='Validate explicit C output with native Clang')
    args = parser.parse_args()
    compiler = str(args.compiler.resolve())
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))
    with tempfile.TemporaryDirectory(prefix='nerd-capabilities-') as directory:
        binary = Path(directory) / ('probe.exe' if os.name == 'nt' else 'probe')
        run(['clang', str(ROOT / 'validation/stdlib/layout.c'), '-o', str(binary)], env)
        native = run([str(binary)], env)
        if native.stderr:
            raise SystemExit(f'Unexpected native layout diagnostic: {native.stderr}')
        for name, expected in [('capabilities', 'stdlib capabilities: passed'),
                               ('layout', native.stdout.strip())]:
            source = ROOT / f'validation/stdlib/{name}.n'
            run([compiler, 'check', str(source)], env)
            for mode in ([], ['--release']):
                output = binary.with_suffix('.c') if args.cgen else binary
                command = [compiler, 'build', *mode, '-o', str(output), str(source)]
                if args.cgen:
                    copts = run([*command, '--cgen', '--copts'], env)
                    run(['clang', str(output), *shlex.split(copts.stdout),
                         '-o', str(binary)], env)
                else:
                    run(command, env)
                result = run([str(binary)], env)
                if result.stdout.strip() != expected or result.stderr:
                    raise SystemExit(f'Unexpected probe output: {result.stdout!r} {result.stderr!r}')
                print(f"PASS {'C' if args.cgen else 'LLVM'} {'release' if mode else 'debug'} {name}")


if __name__ == '__main__':
    main()
