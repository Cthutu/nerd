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
        hello = work / 'hello.n'
        hello.write_text('main :: fn () { prn("Hello, world!") }\n')
        direct = work / 'direct.n'
        direct.write_text('pid :: ffi "c" getpid () -> i32\n'
                          'main :: fn () {\n assert pid() > 0\n prn("libc requested")\n}\n')
        (work / 'libc_dependency.n').write_text('pub pid :: ffi "c" getpid () -> i32\n')
        indirect = work / 'indirect.n'
        indirect.write_text('dep :: use libc_dependency\n'
                            'main :: fn () {\n assert dep.pid() > 0\n prn("libc requested")\n}\n')
        for release in (False, True):
            for backend in ('llvm', 'c'):
                def compile(source, name, independent=True):
                    executable = work / name
                    command = [nerd, 'build', ROOT / source, '-o', executable]
                    if release:
                        command.append('-r')
                    if backend == 'c':
                        flags = shlex.split(run(command + ['--cgen', '--copts'], env).stdout.decode())
                        assert ('-nostdlib' in flags) == independent, flags
                        assert ('-DNRT_FREESTANDING=1' in flags) == independent, flags
                        run(['clang', executable.with_suffix('.c'), *flags, '-o', executable], env)
                    else:
                        run(command, env)
                    if independent:
                        inspect(executable, env)
                    else:
                        dynamic = run(['readelf', '-dW', executable], env).stdout
                        assert b'(NEEDED)' in dynamic and b'libc.so' in dynamic, dynamic
                    return executable
                executable = compile(hello, 'hello')
                run([executable], env, output=b'Hello, world!\n')
                for source in (direct, indirect):
                    executable = compile(source, 'foreign', independent=False)
                    run([executable], env, output=b'libc requested\n')
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
        # Simulate an accidental compiler/runtime libc import. Without source
        # FFI, this must fail at the native link, never retry against libc/CRT.
        tools = work / 'tools'
        tools.mkdir()
        llc = shutil.which('llc')
        assert llc
        wrapper = tools / 'llc'
        wrapper.write_text(f'''#!{sys.executable}
import pathlib, subprocess, sys
status = subprocess.call([{llc!r}, *sys.argv[1:]])
if status == 0 and '-o' in sys.argv:
    output = pathlib.Path(sys.argv[sys.argv.index('-o') + 1])
    data = output.read_bytes()
    symbol = b'nrt_core_init\\0'
    assert symbol in data
    output.write_bytes(data.replace(symbol, b'getenv' + b'\\0' * (len(symbol) - 6)))
sys.exit(status)
''')
        wrapper.chmod(0o755)
        result = run([nerd, 'build', hello, '-o', work / 'unowned'],
            dict(env, PATH=f'{tools}:{env["PATH"]}', NERD_CRT_DIR=str(work / 'no-crt')),
            status=1)
        assert b'getenv' in result.stderr and b'undefined symbol' in result.stderr, result
        assert b'Cannot find native CRT' not in result.stderr, result
        print('PASS implicit libc imports fail without source FFI')
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
