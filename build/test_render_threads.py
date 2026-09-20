#!/usr/bin/env python3
"""Compare actual LLVM module renders across serial and concurrent execution."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

import build as compiler_build
from benchmark_compiler import generate_inputs

ROOT = Path(__file__).resolve().parents[1]


def sanitized_compiler(directory, sanitizer):
    # Use the real project source/define discovery, with isolated instrumented
    # objects. Runtime embedded objects are prepared by the normal build first.
    sources, project_defines = compiler_build.sources_for_project('nerd')
    cc = shlex.split(os.environ.get('CC', 'clang'))
    flags = ['-std=c23', '-g', '-O1', '-DDEBUG', '-I' + str(ROOT / 'src'),
             '-fsanitize=' + sanitizer, '-fno-omit-frame-pointer']
    if os.name != 'nt':
        flags += ['-pthread']
    directory.mkdir()

    def compile_one(item):
        index, source = item
        _, defines = compiler_build.module_config_for_dir(source.parent)
        if source == ROOT / 'src/nerd.c':
            defines = [*defines, *project_defines]
        output = directory / f'{index}.o'
        result = subprocess.run(cc + flags + ['-D' + d for d in defines] +
                                ['-c', str(source), '-o', str(output)],
                                cwd=ROOT, capture_output=True, text=True)
        if result.returncode:
            raise RuntimeError(result.stderr)
        return output

    with ThreadPoolExecutor(max_workers=4) as pool:
        objects = list(pool.map(compile_one, enumerate(sources)))
    output = directory / ('nerd.exe' if os.name == 'nt' else 'nerd')
    subprocess.run(cc + flags + list(map(str, objects)) + compiler_build.LDFLAGS +
                   ['-o', str(output)], cwd=ROOT, check=True)
    return output


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug')
    parser.add_argument('--sanitize', choices=['thread', 'address'])
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='nerd-render-threads-') as directory:
        work = Path(directory)
        nerd = (sanitized_compiler(work / 'compiler', args.sanitize)
                if args.sanitize else args.nerd.resolve())
        inputs = generate_inputs(work, modules=6, functions=8)
        inputs.update(dungeon=ROOT / 'examples/dungeon/dungeon.n',
                      pixels=ROOT / 'examples/pixels/pixels.n',
                      quill=ROOT / 'examples/text-adventure/quill.n')
        for name in ['196-plex-use', '128-generic-functions', '077-enum-discriminants']:
            fixture = ROOT / 'tests/language' / (name + '.t')
            source = work / (name + '.n')
            source.write_text(fixture.read_text().split('¬')[0])
            inputs[name] = source
        imports = work / 'imports'
        imports.mkdir()
        (imports / 'dep.n').write_text('pub Pair :: plex { x i32 y i32 }\n'
            'pub value :: Pair { x: 3 y: 4 }\n'
            'pub read_pair :: fn (p: Pair = Pair { x: 5 y: 6 }) -> i32 { return p.x + p.y }\n')
        (imports / 'main.n').write_text('use dep\n'
            'main :: fn () -> i32 { return read_pair() + value.x }\n')
        inputs['imported-values-defaults'] = imports / 'main.n'
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_PROFILE='1')
        env.pop('NERD_MEMORY_PROFILE', None)
        regression = subprocess.run([str(nerd), 'internal-test', 'llvm-render-sema'],
                                    env=env, capture_output=True, text=True, timeout=60)
        assert regression.returncode == 0, regression.stderr
        for name, source in inputs.items():
            result = subprocess.run([str(nerd), 'internal-test', 'llvm-render-concurrent'],
                                    cwd=source.parent,
                                    env=dict(env, NERD_TEST_RENDER_SOURCE=str(source)),
                                    capture_output=True, text=True, timeout=180)
            assert result.returncode == 0, (name, result.stdout, result.stderr)
            assert result.stdout == 'llvm-render-concurrent ok\n', result.stdout
            print(f'[PASS] concurrent LLVM: {name}', flush=True)
    print('Serial/concurrent LLVM identity, repeated input reuse and cleanup passed')


if __name__ == '__main__':
    main()
