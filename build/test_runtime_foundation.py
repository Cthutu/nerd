#!/usr/bin/env python3
"""Check runtime primitives, Linux I/O and the partial libc-removal boundary."""
import os
from pathlib import Path
import platform
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

# Deliberately not a claim that the complete runtime is freestanding yet.
REMOVED_SYMBOLS = {
    'bcmp', 'memcmp', 'memcpy', 'memmove', 'memset', 'strcmp', 'strlen', 'calloc', 'free',
    'mmap', 'mprotect', 'munmap', 'read', 'write', 'ioctl', 'isatty', 'exit',
    'fwrite', 'fputc', 'fflush', 'vfprintf', 'getc', 'getchar', 'ungetc',
    'stdin', 'stdout', 'stderr', 'abort', '__stack_chk_fail',
}


def check_linux_io(output):
    import pty
    import select
    import signal
    import threading
    import time
    import tty

    def run(mode, data=b''):
        return subprocess.run([str(output), mode], input=data,
                              capture_output=True, timeout=10)

    result = run('output')
    assert (result.returncode, result.stdout, result.stderr) == (
        0, b'A\0B\ntail\n', b'E\0F\nerror\n'), result
    result = run('input', b'a\nbb\r\nccc\rD\n\0\xffz\nlongline\nlast')
    assert (result.returncode, result.stdout, result.stderr) == (
        0, b'a\nbb\nccc\nD\n\0\xffz\nlon\nlas\n\n', b''), result
    for data in (b'discard\nnext\n', b'discard\r\nnext\r', b'discard\rnext'):
        result = run('empty-view', data)
        assert (result.returncode, result.stdout, result.stderr) == (0, b'next\n', b''), result
    result = run('input', b'')
    assert (result.returncode, result.stdout, result.stderr) == (0, b'\n' * 8, b''), result
    result = run('diagnostics')
    assert result.returncode == 0 and result.stdout == result.stderr, result
    assert len(result.stdout) > 1024, result
    result = run('invalid-format')
    assert (result.returncode, result.stdout, result.stderr) == (
        0, b'', b'[invalid runtime diagnostic format]'), result
    for mode in ('bad-output', 'nonblocking-output'):
        result = run(mode)
        assert (result.returncode, result.stdout, result.stderr) == (0, b'', b'survived\n'), result
    result = run('broken-output')
    assert (result.returncode, result.stdout, result.stderr) == (-signal.SIGPIPE, b'', b''), result
    for mode, expected in (('bad-input', b''), ('nonblocking-input', b'next\n'),
                           ('mixed-stdio', b'foreign runtime\n')):
        result = run(mode)
        assert (result.returncode, result.stdout, result.stderr) == (0, expected, b''), result
    for mode, stdout, stderr in (
        ('fatal', b'before failure\n', b'fatal failure\n'),
        ('assert', b'', b'assertion failed at probe.n:42: binary\0message\n'),
        ('worker-fatal', b'', b'worker failure\n'),
    ):
        result = run(mode)
        assert (result.returncode, result.stdout, result.stderr) == (127, stdout, stderr), result
    for mode, stderr in (
        ('signal-default', b''), ('signal-ignore', b''), ('signal-blocked', b''),
        ('signal-handler', b'abort handler\n'), ('signal-worker', b''),
        ('stack-fail', b'fatal: stack corruption detected\n'),
        ('slice-abort', b'fatal: string slice out of bounds\n'),
    ):
        result = run(mode)
        assert (result.returncode, result.stdout, result.stderr) == (-signal.SIGABRT, b'', stderr), result
    result = run('signal-handler-exit')
    assert (result.returncode, result.stdout, result.stderr) == (42, b'', b''), result
    result = run('normal-return')
    assert (result.returncode, result.stdout, result.stderr) == (
        0, b'normal\n', b'foreign atexit\n'), result
    result = subprocess.run([str(output), 'report'], stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=10)
    assert (result.returncode, result.stdout) == (0, b'before\nError: problem\n'), result

    # TCGETS must detect a real terminal, and colour policy must still honour env.
    for no_colour, term, colour in ((None, 'xterm', True), ('1', 'xterm', False),
                                    (None, 'dumb', False), ('', 'xterm', True)):
        master, slave = pty.openpty()
        try:
            tty.setraw(slave)
            env = dict(os.environ, TERM=term)
            env.pop('NO_COLOR', None)
            if no_colour is not None:
                env['NO_COLOR'] = no_colour
            result = subprocess.run([str(output), 'report'], stdout=subprocess.PIPE,
                                    stderr=slave, env=env, timeout=10)
            assert result.returncode == 0 and result.stdout == b'before\n', result
            assert select.select([master], [], [], 5)[0], 'no terminal diagnostic'
            received = os.read(master, 1024)
            expected = b'\x1b[31mError: problem\x1b[0m\n' if colour else b'Error: problem\n'
            assert received == expected, received
        finally:
            os.close(slave)
            os.close(master)

    # Slow draining plus non-restarting signals force interrupted/short writes.
    with subprocess.Popen([str(output), 'write-interrupt'], stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE) as child:
        chunks = []

        def drain():
            while chunk := child.stdout.read(4096):
                chunks.append(chunk)
                time.sleep(0.001)

        reader = threading.Thread(target=drain, daemon=True)
        reader.start()
        try:
            child.wait(timeout=15)
        finally:
            if child.poll() is None:
                child.kill()
                child.wait()
            reader.join(timeout=5)
        assert not reader.is_alive(), 'pipe reader did not finish'
        assert child.returncode == 0, child.stderr.read()
        assert child.stderr.read() == b''
        assert b''.join(chunks) == bytes(i % 251 for i in range(1024 * 1024))
    with subprocess.Popen([str(output), 'read-interrupt'], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE) as child:
        try:
            assert select.select([child.stderr], [], [], 5)[0], 'no read readiness marker'
            assert child.stderr.read(6) == b'ready\n'
            time.sleep(0.03)
            stdout, stderr = child.communicate(b'okay\n', timeout=10)
            assert (child.returncode, stdout, stderr) == (0, b'okay\n', b'')
        finally:
            if child.poll() is None:
                child.kill()
                child.wait()


def main():
    cc = shlex.split(os.environ.get('CC', 'clang'))
    raw_linux = platform.system() == 'Linux' and platform.machine() in {
        'x86_64', 'aarch64', 'arm64',
    }
    with tempfile.TemporaryDirectory(prefix='nerd-runtime-foundation-') as directory:
        directory = Path(directory)
        output = directory / ('runtime.exe' if os.name == 'nt' else 'runtime')
        if raw_linux:
            for optimisation in ('-O0', '-O2'):
                for name, libraries in (('format', []), ('maths', ['-ldl', '-lm'])):
                    oracle = directory / ('runtime-' + name)
                    subprocess.run(cc + ['-std=c23', '-Wall', '-Wextra', '-Werror', '-fno-builtin', optimisation,
                        str(ROOT / ('tests/core/runtime-' + name + '.linux.c')), *libraries, '-o', str(oracle)],
                        check=True, timeout=60)
                    result = subprocess.run([str(oracle)], capture_output=True, timeout=30)
                    assert (result.returncode, result.stdout, result.stderr) == (
                        0, ('runtime ' + name + ' passed\n').encode(), b''), result
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
                    io_output = directory / 'runtime-io'
                    for executable_metadata in (False, True):
                        variant = ['-DNRT_LINUX_EXECUTABLE=1'] if executable_metadata else []
                        subprocess.run(cc + flags + variant + [optimisation, '-pthread', '-fstack-protector-all',
                            str(ROOT / 'tests/core/runtime-io.linux.c'), '-o', str(io_output)],
                            check=True, timeout=60)
                        check_linux_io(io_output)
                    metadata_output = directory / 'runtime-metadata'
                    subprocess.run(cc + flags + [optimisation, '-DNRT_LINUX_EXECUTABLE=1',
                        str(ROOT / 'tests/core/runtime-metadata.linux.c'),
                        '-o', str(metadata_output)], check=True, timeout=60)
                    result = subprocess.run([str(metadata_output)], capture_output=True, timeout=10)
                    assert (result.returncode, result.stdout, result.stderr) == (
                        0, b'runtime metadata passed\n', b''), result
            if raw_linux:
                # Match the optimised embedded runtime build, including PIC.
                # Inspect nrt alone: the host test oracle intentionally uses libc.
                for variant in ([], ['-DNRT_LINUX_EXECUTABLE=1', '-DNRT_FREESTANDING=1',
                                      '-ftls-model=local-exec', '-ffreestanding', '-fno-builtin']):
                    for pic in (False, True):
                        obj = directory / 'nrt.o'
                        subprocess.run(cc + flags + variant + ['-O2', '-fstack-protector-strong'] + (['-fPIC'] if pic else []) + [
                            '-c', str(ROOT / 'data/nrt.c'), '-o', str(obj)],
                            check=True, timeout=60)
                        symbols = subprocess.check_output(['nm', '-u', str(obj)], text=True)
                        undefined = {line.split()[-1].split('@')[0]
                                     for line in symbols.splitlines() if line.split()}
                        removed = REMOVED_SYMBOLS | {'snprintf', 'vsnprintf'} | ({'getenv', 'sysconf', '__tls_get_addr'} if variant else set())
                        assert not undefined & removed, symbols
                        if variant:
                            assert undefined <= {'main', '__preinit_array_start', '__preinit_array_end',
                                '__init_array_start', '__init_array_end', '__fini_array_start', '__fini_array_end'}, symbols
                        sections = subprocess.check_output(['readelf', '-SW', str(obj)], text=True)
                        assert '.preinit_array' not in sections, sections
                        # The compiler ABI hook is runtime-owned but never exported.
                        table = subprocess.check_output(['readelf', '-Ws', str(obj)], text=True)
                        hook = [line.split() for line in table.splitlines()
                                if line.split() and line.split()[-1] == '__stack_chk_fail']
                        assert len(hook) == 1 and hook[0][3:6] == ['FUNC', 'GLOBAL', 'HIDDEN'], table
                        relocations = subprocess.check_output(['readelf', '-rW', str(obj)], text=True)
                        assert '__stack_chk_fail' in relocations, 'compiler stack checks were not emitted'
            print('[PASS] runtime foundation:', 'release' if release else 'debug')


if __name__ == '__main__':
    main()
