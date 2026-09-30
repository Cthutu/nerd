#!/usr/bin/env python3
"""Source build settings: module isolation, precedence and native linking."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin' /
                        ('nerd-debug.exe' if os.name == 'nt' else 'nerd-debug'))
    args = parser.parse_args()
    nerd = str(args.nerd.resolve())
    with tempfile.TemporaryDirectory(prefix='nerd-build-settings-') as directory:
        work = Path(directory)
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'),
                   NERD_TEST_SDK=str(work / 'sdk with spaces'))
        env.pop('NERD_TEST_MISSING_SDK', None)

        def write(name, text):
            path = work / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding='utf-8', newline='\n')
            return path

        def run(*command, ok=True):
            result = subprocess.run([nerd, *map(str, command)], cwd=work,
                                    env=env, capture_output=True, text=True, encoding="utf-8")
            assert (result.returncode == 0) == ok, (command, result.stdout, result.stderr)
            return result.stdout + result.stderr

        root = write('main.n', '''build {
    on "later" { library_path: $NERD_TEST_MISSING_SDK }
    define: later
    define: local_feature
    windowed: yes
    windowed: no
    library_path: "root libs"
    library_path: "root libs"
    on "later" { library_path: $NERD_TEST_SDK/Lib }
}
left :: use left
right :: use right
on "local_feature" { main :: fn () => left.value() + right.value() - 42 }
''')
        write('left.n', '''build { library_path: "left libs" }
common :: use common
on "local_feature" { invalid :: missing_symbol }
pub value :: fn () => common.value()
''')
        write('right.n', '''build { define: child_feature library_path: "right libs" }
common :: use common
on "child_feature" { pub value :: fn () => common.value() }
''')
        write('common.n', '''build { windowed: yes library_path: "common libs" }
pub value :: fn () => 21
''')
        outputs = [run('build', '--copts', '--jobs', jobs, root) for jobs in (1, 4)]
        assert outputs[0] == outputs[1], outputs
        flags = outputs[0]
        assert flags.count('root libs') == 1, flags
        assert flags.index('root libs') < flags.index('left libs') < flags.index('right libs') < flags.index('common libs'), flags
        assert 'sdk with spaces/Lib' in flags.replace('\\', '/'), flags
        assert 'SUBSYSTEM:WINDOWS' not in flags, flags
        run('run', root)
        # Defines remain local even when they enable conditional imports.
        write('main.n', 'build { define: local_feature }\non "local_feature" { left :: use left }\nmain :: fn () => left.value() - 21\n')
        run('run', root)
        write('main.n', 'right :: use right\non "child_feature" { invalid :: missing_symbol }\nmain :: fn () {}\n')
        run('check', root)
        # Sibling conflicts need an explicit importer override.
        write('left.n', 'build { windowed: no }\n')
        write('right.n', 'build { windowed: yes }\n')
        write('main.n', 'left :: use left\nright :: use right\nmain :: fn () {}\n')
        assert 'Conflicting imported windowed' in run('build', '--copts', root, ok=False)
        write('main.n', 'build { windowed: no }\nleft :: use left\nright :: use right\nmain :: fn () {}\n')
        run('build', '--copts', root)
        # Missing environment is a link-time error, not an editor dependency.
        write('main.n', 'build { library_path: $NERD_TEST_MISSING_SDK/Lib }\nmain :: fn () {}\n')
        run('check', root)
        run('build', '--cgen', root)
        run('build', '--obj', root)
        run('build', '--lib', root)
        assert 'NERD_TEST_MISSING_SDK is not set' in run('build', '--copts', root, ok=False)
        for entry in ('unknown: yes', 'windowed: "yes"', 'define: 42', 'library_path: ["x"]', 'library_path: $ SDK'):
            write('main.n', f'build {{ {entry} }}\nmain :: fn () {{}}\n')
            run('check', root, ok=False)
        write('main.n', 'build {}\nbuild { on \"cli_feature\" { define: source_feature } }\non \"source_feature\" { main :: fn () {} }\n')
        run('-Dcli_feature', 'check', root)
        # Contextual keyword remains available as an ordinary binding.
        write('main.n', 'build :: 42\nmain :: fn () => build - 42\n')
        run('run', root)
        # Real library lookup with two implementations proves root search order.
        for name, value in (('override', 42), ('sdk with spaces/Lib', 7)):
            folder = work / name
            folder.mkdir(parents=True, exist_ok=True)
            source = write(f'{name}/fixture.c', f'int nerd_fixture(void) {{ return {value}; }}\n')
            obj = folder / 'fixture.o'
            subprocess.run(['clang', '-c', str(source), '-o', str(obj)], check=True, env=env)
            archive = folder / ('nerd_fixture.lib' if os.name == 'nt' else 'libnerd_fixture.a')
            subprocess.run(['llvm-ar', 'rcs', str(archive), str(obj)], check=True, env=env)
        write('native.n', '''build { library_path: $NERD_TEST_SDK/Lib }
ffi "nerd_fixture" { pub nerd_fixture () -> i32 }
''')
        write('main.n', 'build { library_path: "override" }\nnative :: use native\nmain :: fn () => native.nerd_fixture() - 42\n')
        run('run', root)
        write('main.n', 'native :: use native\nmain :: fn () => native.nerd_fixture() - 7\n')
        run('run', root)
        # Folder parts resolve paths relative to the declaring part.
        write('parts/mod.n', 'pub value :: fn () => 0\n')
        write('parts/settings.n', 'build { library_path: "part libs" }\n')
        write('main.n', 'parts :: use parts\nmain :: fn () {}\n')
        flags = run('build', '--copts', root).replace('\\', '/')
        assert '/parts/part libs' in flags, flags
    print('Source build settings: passed')


if __name__ == '__main__':
    main()
