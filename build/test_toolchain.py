#!/usr/bin/env python3
"""Integration checks for direct LLVM tools and nerd doctor."""
import argparse
from test import fixture_source
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
        # These checks compare diagnostic content, including long native paths.
        # Keep presentation wrapping independent of the host terminal width.
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), TMPDIR=str(work),
                   COLUMNS='4096')
        tools = ['opt', 'llc', 'llvm-lib' if os.name == 'nt' else 'llvm-ar',
                 'lld-link' if os.name == 'nt' else
                 'ld64.lld' if sys.platform == 'darwin' else 'ld.lld']
        if os.name == 'nt':
            # No Clang/CL on the native build PATH. Hard links avoid copying a
            # full LLVM distribution and require no symlink privilege.
            bindir = work / 'tools'
            bindir.mkdir()
            for name in tools:
                resolved = shutil.which(name)
                assert resolved, f'Missing required test dependency: {name}'
                try:
                    os.link(resolved, bindir / (name + '.exe'))
                except OSError:
                    shutil.copy2(resolved, bindir / (name + '.exe'))
            env['PATH'] = str(bindir)
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
                                    env=env, capture_output=True, text=True, encoding="utf-8")
            assert (result.returncode == 0) == success, result.stdout + result.stderr
            assert 'forbidden-clang-invocation' not in result.stdout + result.stderr
            return result.stdout + result.stderr

        # Definition-location notes and explanatory notes share cyan prefixes.
        note_source = work / 'missing-trait.n'
        note_source.write_text('Hash :: plex { bytes [20]u8 }\n'
                               'impl Display for Hash {}\n', encoding='utf-8')
        diagnostic = run('check', note_source, success=False)
        for message in ('Type `Display` is defined here', 'Missing member: `show`'):
            assert '\x1b[1;36mnote:\x1b[0m ' + message in diagnostic, diagnostic
        print('[PASS] definition and explanatory note colours')

        # A missing .n argument is a filename, even when it never existed.
        # Fail before parsing, generating output or invoking a native tool.
        for missing_source in ['missing.n', 'missing folder/file.n',
                               'examples/absent/absent.n', str(work / 'absent.n')]:
            for command in [('build',), ('build', '-r'), ('build', '--cgen'),
                            ('build', '--copts'), ('check',), ('run',)]:
                failure = run(*command, missing_source, success=False)
                assert f'Source file not found: {missing_source}' in failure, failure
                assert 'EOF' not in failure and '<input>' not in failure, failure
        failure = run('build', str(work), success=False)
        assert 'Expected a source file but found directory:' in failure, failure
        assert 'EOF' not in failure, failure
        # Existing files with nonstandard extensions still work, and an inline
        # program containing a filename string must remain an inline program.
        unusual = work / 'program.source'
        unusual.write_text('main :: fn () -> i32 { return 0 }\n')
        run('check', unusual)
        run('build', '--cgen', 'main :: fn () { assert "missing.n".count == 9 }',
            '-o', work / 'inline.c')
        assert (work / 'inline.c').exists()
        run('build', '--cgen',
            'Value :: plex { n i32 }\nvalue :: Value { n: 7 }\nmain :: fn () => value.n',
            '-o', work / 'inline-field.c')
        assert (work / 'inline-field.c').exists()
        # Qualified imports and function-result binders keep source record names
        # in diagnostics, including transitively imported and composite types.
        records = work / 'diagnostic_records'
        records.mkdir()
        (records / 'mod.n').write_text(
            'pub Widget :: plex { value u64 }\n'
            'pub Bits :: union { number u64 byte u8 }\n'
            'pub make :: fn () -> Widget { return Widget { value: 7 } }\n')
        relay = work / 'diagnostic_relay'
        relay.mkdir()
        (relay / 'mod.n').write_text(
            'records :: use diagnostic_records\n'
            'pub make :: fn () -> records.Widget { return records.make() }\n')
        diagnostic_source = work / 'type-names.n'
        cases = [
            ('return records.make()', 'Widget'),
            ('value := records.Widget { value: 7 }\nreturn ^value', '^Widget'),
            ('value := records.Bits { number: 7 }\nreturn value', 'Bits'),
            ('return relay.make()', 'Widget'),
            ('value := Local { value: 7 }\nreturn records.make()', 'Local'),
        ]
        for body, expected in cases:
            diagnostic_source.write_text(
                'records :: use diagnostic_records\n'
                'relay :: use diagnostic_relay\n'
                + ('Local :: plex { value u64 }\n' if expected == 'Local' else '')
                + 'main :: fn () -> i32 {\n' + body + '\n}\n')
            failure = run('check', diagnostic_source, success=False)
            assert f'Type mismatch: expected `i32`, found `{expected}`' in failure, failure
            assert f'This expression has type `{expected}`' in failure, failure
        diagnostic_source.write_text(
            'signal :: use std.signal\n'
            'main :: fn () -> i32 {\n'
            'on signal.watch_shutdown() {\n'
            'watcher => return watcher\nerror! => return 1\n}\n}\n')
        failure = run('check', diagnostic_source, success=False)
        assert 'Type mismatch: expected `i32`, found `ShutdownWatcher`' in failure, failure

        # Suggest break only for a mismatched whole return value that can
        # supply the nearest expression block. Nested functions/loops and
        # errors inside call arguments must retain their ordinary diagnostics.
        break_cases = [
            ('value: string = ${ return "text" }', True),
            ('value := ${ return "text" }', True),
            ('value: bool = ${ return "text" }', False),
            ('value: string = ${ for { return "text" } }', False),
            ('value: string = ${ worker :: fn () -> i32 { return "text" }\nbreak "text" }', False),
            ('value: string = ${ return take("text") }', False),
            ('for { value: string = ${ return "text" } }', True),
        ]
        for body, suggest_break in break_cases:
            diagnostic_source.write_text(
                'take :: fn (value: i32) -> i32 { return value }\n'
                'main :: fn () -> i32 {\n' + body + '\nreturn 0\n}\n')
            failure = run('check', diagnostic_source, success=False)
            assert ('Did you mean `break` instead of `return`' in failure) == suggest_break, failure
            assert 'Type mismatch' in failure, failure
        diagnostic_source.write_text(
            'signal :: use std.signal\n'
            'main :: fn () -> i32 {\n'
            'watcher := on signal.watch_shutdown() => [value] ${ return value }\n'
            'else [error] { return 1 }\nreturn 0\n}\n')
        failure = run('check', diagnostic_source, success=False)
        assert 'found `ShutdownWatcher`' in failure, failure
        assert 'Did you mean `break` instead of `return`' in failure, failure
        diagnostic_source.write_text(
            'main :: fn () -> i32 {\n'
            '_value: string = ${ return 7 }\nreturn 0\n}\n')
        run('check', diagnostic_source)  # A valid early function return remains valid.

        # Bare handlers require an optional/result and retain propagation channel
        # checks. An on subject handler still needs a branch; else? is rejected.
        shorthand_prelude = (
            'Failure :: enum { Bad }\nOther :: enum { Different }\n'
            'operation :: fn () -> i32\\Failure { return Failure.Bad! }\n'
            'optional :: fn () -> ?i32 { return nil }\n')
        shorthand_failures = [
            ('i32', 'value := operation() else {}\nreturn value', 'propagat'),
            ('i32', 'value := operation() else { on no => return 2 }\nreturn value', 'propagat'),
            ('i32\\Other', 'value := operation() else {}\nreturn value', 'found `Failure`'),
            ('i32', 'value := 5 else { return 2 }\nreturn value', 'optional or result value'),
            ('i32', 'value := optional() else [error] { return 2 }\nreturn value', 'optional'),
            ('i32\\Failure', 'value := operation() else return 2\nreturn value', 'Expected LeftBrace'),
            ('i32\\Failure', 'value := on operation() else {}\nreturn value', 'Expected FatArrow'),
            ('i32\\Failure', 'value := on operation() else? {}\nreturn value', 'Expected LeftBrace'),
        ]
        for return_type, body, expected in shorthand_failures:
            diagnostic_source.write_text(shorthand_prelude +
                'check_value :: fn () -> ' + return_type + ' {\n' + body + '\n}\n'
                'main :: fn () {}\n')
            failure = run('check', diagnostic_source, success=False)
            assert expected in failure, failure
            assert 'LLVM lowering failed' not in failure, failure

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
            source.write_text(fixture_source(name))
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

        # The host wrapper must call the exact Nerd return type before converting
        # to a process status. POSIX truncation hides dirty upper bits on narrow
        # returns, so assert the full DWORD status on Windows.
        for kind, value in [('i8', -7), ('u8', 232), ('i16', -300),
                            ('u16', 60000), ('i64', -4294967303), ('u64', 4294967528)]:
            expected = value & (0xffffffff if os.name == 'nt' else 0xff)
            for parameters in ['', 'args: []string']:
                body = ('assert args.count >= 1\n' if parameters else '') + f'return {value}.as({kind})'
                for windowed in ([False, True] if os.name == 'nt' and not parameters else [False]):
                    source.write_text(('build { windowed: yes }\n' if windowed else '') +
                                      f'main :: fn ({parameters}) {{ {body} }}\n')
                    for release in [[], ['-r']]:
                        run('build', *release, '--jobs', 4 if parameters else 1, source, '-o', executable)
                        result = subprocess.run([str(executable)], env=env, capture_output=True)
                        assert result.returncode == expected, (kind, parameters, windowed, release, result.returncode, expected, result.stderr)
        print('[PASS] entry-point integer widths, signedness, arguments and target modes')
        # Heap enum stores must remain aligned after header placement, growth,
        # and explicit reserve. Optimized Windows code uses aligned SIMD stores.
        source.write_text(fixture_source('317-run-dynamic-enum-alignment'), encoding="utf-8")
        for release in [[], ['-r']]:
            for jobs in [1, 4]:
                run('build', *release, '--jobs', jobs, source, '-o', executable)
                result = subprocess.run([str(executable)], env=env, capture_output=True)
                assert result.returncode == 0, (release, jobs, result.returncode, result.stderr)
        print('[PASS] dynamic enum allocation, growth and reserve alignment at debug/release jobs 1/4')
        source.write_text('main :: fn () -> i32 { return 0 }\n')

        if os.name == 'nt':
            for missing in tools:
                original = bindir / (missing + '.exe')
                hidden = original.with_suffix('.disabled')
                original.rename(hidden)
                try:
                    assert f'[ERROR] {missing}' in run('doctor', success=False)
                finally:
                    hidden.rename(original)
            previous_lib = env.get('LIB')
            env['LIB'] = str(work / 'absent-sdk')
            assert '[ERROR]' in run('doctor', success=False)
            if previous_lib is None:
                del env['LIB']
            else:
                env['LIB'] = previous_lib
            empty = work / 'empty'
            empty.mkdir()
            env['PATH'] = str(empty)
            failure = run('doctor', success=False)
            assert all(f'[ERROR] {name}' in failure for name in tools), failure
            failure = run('build', source, '-o', work / 'missing-tool', success=False)
            assert 'LLVM tool failed' in failure and 'llc' in failure
            assert 'internal compiler error' not in failure
            run('check', source)
            run('build', '--cgen', source, '-o', work / 'compatible.c')
            assert (work / 'compatible.c').exists()

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
