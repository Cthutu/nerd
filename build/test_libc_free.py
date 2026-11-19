#!/usr/bin/env python3
"""Behaviour and ELF acceptance gate for independent Linux executables."""
import argparse
import os
from pathlib import Path
import shutil
import shlex
import signal
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run(command, env, *, status=0, output=None, cwd=ROOT, prepare=None):
    result = subprocess.run([str(c) for c in command], cwd=cwd, env=env,
                            capture_output=True, timeout=60, preexec_fn=prepare)
    assert result.returncode == status, (command, result)
    if output is not None:
        assert result.stdout == output and not result.stderr, (command, result)
    return result


def inspect(executable, env):
    headers = run(['readelf', '-lW', executable], env).stdout
    dynamic = run(['readelf', '-dW', executable], env).stdout
    assert b'INTERP' not in headers, headers
    assert b'(NEEDED)' not in dynamic, dynamic
    assert not run(['nm', '-u', executable], env).stdout.strip()
    assert b' TLS ' in headers, headers


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, required=True)
    args = parser.parse_args()
    if not sys.platform.startswith('linux'):
        print('SKIP independent ELF startup requires Linux')
        return
    import resource
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NO_COLOR='1', TERM='dumb')
    nerd = args.nerd.resolve()
    (ROOT / '_tmp').mkdir(exist_ok=True)
    temporary = tempfile.mkdtemp(prefix='libc-free-', dir=ROOT / '_tmp')
    try:
        work = Path(temporary)
        provider = work / 'provider'
        provider.write_text('printf "exec:%s:%s\\n" "$1" "$LIBC_FREE_ENV"\nexit 23\n')
        provider.chmod(0o755)  # Deliberately no #!: exercise ENOEXEC shell fallback.
        denied = work / 'denied'
        denied.mkdir()
        (denied / 'provider').write_text('not executable')
        execution_env = dict(env, PATH=f'{denied}:{work}', LIBC_FREE_ENV='inherited')
        for release in (False, True):
            for backend in ('llvm', 'c'):
                def compile(source, name):
                    executable = work / name
                    command = [nerd, 'build', '--freestanding', ROOT / source, '-o', executable]
                    if release:
                        command.append('-r')
                    if backend == 'c':
                        flags = shlex.split(run(command + ['--cgen', '--copts'], env).stdout.decode())
                        assert '-nostdlib' in flags and '-DNRT_FREESTANDING=1' in flags, flags
                        run(['clang', executable.with_suffix('.c'), *flags, '-o', executable], env)
                    else:
                        run(command, env)
                    inspect(executable, env)
                    return executable
                executable = compile('tests/core/libc-free.n', 'language')
                run([executable, 'hello', work / 'bytes'], env, status=42,
                    output=b'libc-free language passed\n')
                executable = compile('tests/core/libc-free-exec.n', 'exec')
                for filename in ('provider', str(provider)):
                    run([executable, filename], execution_env, status=23,
                        output=b'exec:payload:inherited\n')
                run([executable, 'provider'], dict(execution_env, PATH=''), cwd=work,
                    status=23, output=b'exec:payload:inherited\n')
                run([executable, 'missing'], execution_env, status=2, output=b'')
                run([executable, 'provider'], dict(execution_env, PATH=str(denied)), status=13, output=b'')
                run([executable, 'x' * 5000], execution_env, status=36, output=b'')
                default_path = dict(execution_env)
                default_path.pop('PATH')
                run([executable, 'true'], default_path, output=b'')
                # Loopback execution remains opt-in; the default gate inspects
                # this real socket/readiness program without opening sockets.
                compile('tests/network/readiness.n', 'network')
                executable = compile('tests/stdlib-thread-sync/lifecycle.n', 'thread')
                run([executable], env, output=b'Thread lifecycle and predicate waits passed\n')
                print('PASS libc-free:', backend, 'release' if release else 'debug')
        for optimisation in ('-O0', '-O2'):
            executable = work / 'startup'
            run(['clang', '-std=c23', '-Wall', '-Wextra', '-Werror', optimisation,
                 '-DNRT_LINUX_EXECUTABLE=1', '-DNRT_FREESTANDING=1', '-ffreestanding', '-fno-builtin',
                 '-ftls-model=local-exec', '-fstack-protector-all', '-ffunction-sections', '-fdata-sections',
                 '-nostdlib', '-no-pie', '-fuse-ld=lld', '-Wl,--gc-sections,--no-dynamic-linker,-e,_start',
                 ROOT / 'tests/core/runtime-start.linux.c', '-o', executable], env)
            inspect(executable, env)
            run([executable], env, status=42, output=b'native startup passed\nnative fini passed\n')
            for mode, status in [('abort', -signal.SIGABRT), ('worker-abort', -signal.SIGABRT),
                                 ('guard', -signal.SIGABRT), ('worker-fatal', 127)]:
                result = run([executable, mode], env, status=status)
                assert not result.stdout, result
                if mode == 'guard':
                    assert b'fatal: stack corruption detected' in result.stderr, result
            for prepare in (lambda: signal.signal(signal.SIGABRT, signal.SIG_IGN),
                            lambda: signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGABRT})):
                for mode in ('abort', 'worker-abort'):
                    result = run([executable, mode], env, status=-signal.SIGABRT, prepare=prepare)
                    assert not result.stdout, result
            print('PASS native startup/TLS/canaries/worker termination:', optimisation)
    except BaseException:
        print('Failed libc-free artefacts retained:', temporary, file=sys.stderr)
        raise
    else:
        shutil.rmtree(temporary)
    print('Libc-free ELF and behavioural acceptance passed')


if __name__ == '__main__':
    main()
