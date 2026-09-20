#!/usr/bin/env python3
"""Integration checks for direct LLVM tools and nerd doctor."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent.parent


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug')
    nerd = parser.parse_args().nerd.resolve()
    with tempfile.TemporaryDirectory(prefix='nerd toolchain-') as temporary:
        work = Path(temporary)
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), TMPDIR=str(work))
        tools = ['opt', 'llc', 'llvm-lib' if os.name == 'nt' else 'llvm-ar',
                 'lld-link' if os.name == 'nt' else
                 'ld64.lld' if sys.platform == 'darwin' else 'ld.lld']
        if os.name != 'nt':
            # Only LLVM tools are reachable. A trap additionally proves that a
            # hidden C compiler fallback cannot silently pass the smoke tests.
            bindir = work / 'tools'
            bindir.mkdir()
            for name in tools + (['xcrun'] if sys.platform == 'darwin' else []):
                resolved = shutil.which(name)
                assert resolved, f'Missing required test dependency: {name}'
                (bindir / name).symlink_to(resolved)
            (bindir / 'clang').write_text('#!/bin/sh\necho forbidden-clang-invocation >&2\nexit 99\n')
            (bindir / 'clang').chmod(0o755)
            env['PATH'] = str(bindir)

        def run(*args, success=True):
            result = subprocess.run([str(nerd), *map(str, args)], cwd=work,
                                    env=env, capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stdout + result.stderr
            assert 'forbidden-clang-invocation' not in result.stdout + result.stderr
            return result.stdout + result.stderr

        assert 'Toolchain ready' in run('doctor')
        source = work / 'program.n'
        source.write_text('main :: fn () -> i32 { return 0 }\n')
        library = work / 'library.n'
        library.write_text('pub answer :: fn () -> i32 { return 42 }\n')
        for release in [[], ['-r']]:
            executable = work / 'program.exe'
            run('build', *release, source, '-o', executable)
            subprocess.run([str(executable)], check=True, env=env)
            for mode, suffix in [('obj', '.o'), ('lib', '.lib' if os.name == 'nt' else '.a'),
                                 ('dll', '.dll' if os.name == 'nt' else '.dylib' if sys.platform == 'darwin' else '.so')]:
                output = work / ('library' + ('-release' if release else '') + suffix)
                run('build', *release, '--' + mode, library, '-o', output)
                assert output.exists() and output.stat().st_size > 0
                if mode == 'dll':
                    # Separate process releases the DLL before temporary cleanup
                    # on Windows and checks the public ABI without a C compiler.
                    subprocess.run([sys.executable, '-c',
                        'import ctypes,sys; assert ctypes.CDLL(sys.argv[1]).answer() == 42',
                        str(output)], check=True)
        # Exercise opt on real ownership, atomic and variadic lowering, not just
        # a constant-return program. Both target modes must retain behavior.
        for name in ['242-run-atomic-operators', '298-run-variadic-promotions',
                     '308-run-shutdown-watcher', '316-run-pixel-layer-layout']:
            fixture = ROOT / 'tests/commands' / (name + '.cmd')
            source.write_text(fixture.read_text().split('¬')[0])
            behaviors = []
            for release in [[], ['-r']]:
                run('build', *release, '--jobs', 4, source, '-o', executable)
                result = subprocess.run([str(executable)], env=env, capture_output=True)
                behaviors.append((result.returncode, result.stdout, result.stderr))
            assert behaviors[0] == behaviors[1], (name, behaviors)
            assert behaviors[0][0] == 0, (name, behaviors)
        source.write_text('main :: fn () -> i32 { return 0 }\n')
        assert not list(work.glob('*.obj.o'))
        assert not list(work.glob('*.opt.bc'))
        assert not list(work.glob('nerd-doctor-*'))

        if os.name != 'nt':
            for missing in tools:
                original = (bindir / missing).readlink()
                (bindir / missing).unlink()
                assert f'[ERROR] {missing}' in run('doctor', success=False)
                (bindir / missing).symlink_to(original)
            if sys.platform.startswith('linux'):
                env['NERD_CRT_DIR'] = str(work / 'absent-sdk')
                assert '[ERROR] libc startup objects' in run('doctor', success=False)
                del env['NERD_CRT_DIR']
            opt_path = bindir / 'opt'
            real_opt = opt_path.readlink()
            opt_path.unlink()
            opt_path.write_text('#!/bin/sh\nif [ "$1" = "--version" ]; then exit 0; fi\necho incompatible-opt >&2\nexit 1\n')
            opt_path.chmod(0o755)
            failure = run('doctor', success=False)
            assert 'incompatible-opt' in failure
            assert not list(work.glob('nerd-doctor-*'))
            opt_path.unlink()
            opt_path.symlink_to(real_opt)
            empty = work / 'empty'
            empty.mkdir()
            env['PATH'] = str(empty)
            output = run('doctor', success=False)
            for name in tools:
                assert f'[ERROR] {name}' in output
            failure = run('build', source, '-o', work / 'missing-tool', success=False)
            assert 'LLVM tool failed' in failure and 'llc' in failure
            assert 'internal compiler error' not in failure
            run('check', source)
            run('build', '--cgen', source, '-o', work / 'compatible.c')
            assert (work / 'compatible.c').exists()
        assert not list(work.glob('nerd-doctor-*'))
    print('Direct LLVM outputs, no-Clang execution, and doctor dependency checks passed')


if __name__ == '__main__':
    main()
