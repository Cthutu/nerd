#!/usr/bin/env python3
"""Run bounded, single-thread M0 language probes against explicit checkout modules."""
import argparse
import os
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
    args = parser.parse_args()
    compiler = str(args.compiler.resolve())
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))
    source = ROOT / 'validation/stdlib/capabilities.n'
    run([compiler, 'check', str(source)], env)
    with tempfile.TemporaryDirectory(prefix='nerd-capabilities-') as directory:
        binary = Path(directory) / ('probe.exe' if os.name == 'nt' else 'probe')
        for mode in ([], ['--release']):
            run([compiler, 'build', *mode, '-o', str(binary), str(source)], env)
            result = run([str(binary)], env)
            if result.stdout.strip() != 'stdlib capabilities: passed' or result.stderr:
                raise SystemExit(f'Unexpected probe output: {result.stdout!r} {result.stderr!r}')
            print(f"PASS {'release' if mode else 'debug'} capabilities")


if __name__ == '__main__':
    main()
