#!/usr/bin/env python3
"""Check scheduled front-end ownership, graph identity and diagnostic parity."""
import argparse
import json
import os
from pathlib import Path
import random
import shlex
import subprocess
import tempfile

from benchmark_compiler import generate_inputs
from test_render_threads import sanitized_compiler

ROOT = Path(__file__).resolve().parents[1]
PREFIX = 'nerd-profile\t'


def overlap(records, phase):
    intervals = sorted((r['start_ns'], r['start_ns'] + r['wall_ns'])
                       for r in records if r.get('phase') == phase)
    return any(right[0] < left[1] for left, right in zip(intervals, intervals[1:]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path,
                        default=ROOT / '_bin' / ('nerd-debug.exe' if os.name == 'nt' else 'nerd-debug'))
    parser.add_argument('--sanitize', choices=['thread', 'address'])
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='nerd-front-threads-') as directory:
        work = Path(directory)
        nerd = (sanitized_compiler(work / 'compiler', args.sanitize)
                if args.sanitize else args.nerd.resolve())
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_PROFILE='1',
                   NERD_DEBUG_KEEP_LINK_LLVM='1')
        for name in ['NERD_MEMORY_PROFILE', 'NERD_PROFILE_LOCKS', 'NERD_DEBUG_LLVM_SIDECARS']:
            env.pop(name, None)
        output = work / 'program.exe'

        def run(source, jobs, flags=(), library=None):
            settings = env if library is None else dict(env, NERD_LIB_PATH=library)
            result = subprocess.run([str(nerd), 'build', '--jobs', str(jobs),
                                     '--hir', *([] if '--cgen' in flags else ['--llvm']), *flags, str(source), '-o', str(output)],
                                    env=settings, capture_output=True, text=True, timeout=180)
            records = [json.loads(line[len(PREFIX):]) for line in result.stderr.splitlines()
                       if line.startswith(PREFIX)]
            stderr = '\n'.join(line for line in result.stderr.splitlines()
                               if not line.startswith(PREFIX))
            assert 'WARNING: ThreadSanitizer' not in stderr and 'AddressSanitizer' not in stderr, stderr
            artifacts = {p.name: p.read_bytes() for p in work.iterdir()
                         if p.suffix in ['.ll', '.hir', '.c']}
            labels = [(r.get('kind'), r.get('phase'), r.get('module'),
                       r.get('dependency'), r.get('success')) for r in records
                      if r.get('kind') not in ('scheduler', 'scheduler-policy')]
            return result.returncode, result.stdout, stderr, artifacts, labels, records

        def case(name, files):
            folder = work / name
            folder.mkdir()
            for path, text in files.items():
                target = folder / path
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_text(text)
            return folder / 'main.n'

        valid = []
        valid.append(case('diamond-generic-ffi', {
            'leaf.n': 'pub id :: fn [T] (value: T) -> T { return value }\n'
                      'pub ffi "c" { absolute :: abs (value: i32) -> i32 }\n',
            'left.n': 'use leaf\npub value :: fn () -> i32 { return absolute(id[i32](-7)) }\n',
            'right.n': 'use leaf\npub value :: fn () -> i32 { _x := id[i64](7)\n return 7 }\n',
            'main.n': 'left :: use left\nright :: use right\nduplicate :: use left\n'
                      'main :: fn () -> i32 { return left.value() + right.value() + duplicate.value() - 21 }\n',
        }))
        valid.append(case('conditional-parts', {
            'pack/mod.n': 'pub first :: fn () -> i32 { return second() }\n',
            'pack/part.n': 'second :: fn () -> i32 { return 0 }\n',
            'main.n': 'on "debug" { pack :: use pack }\non "release" { pack :: use pack }\n'
                      'on "never_enabled" { missing :: use missing }\n'
                      'main :: fn () -> i32 { return pack.first() }\n',
        }))
        runtime_cases = valid[:]
        diamond = valid[0].parent
        for name, body in [
            ('explicit', 'on id[i64](4294967303) != 4294967303 => return 99\n return 7'),
            ('inferred', 'x: i64 = 4294967303\n on id(x) != 4294967303 => return 99\n return 7'),
            ('function-value', 'f := id[i64]\n on f(4294967303) != 4294967303 => return 99\n return 7'),
        ]:
            files = {p.name: p.read_text() for p in diamond.glob('*.n')}
            files['right.n'] = 'use leaf\npub value :: fn () -> i32 { ' + body + ' }\n'
            source = case('diamond-' + name, files)
            valid.append(source)
            runtime_cases.append(source)
        inputs = generate_inputs(work / 'shapes', modules=12, functions=32)
        valid += list(inputs.values())
        for fixture in ['128-generic-functions', '196-plex-use', '077-enum-discriminants']:
            source = work / (fixture + '.n')
            source.write_text((ROOT / 'tests/language' / (fixture + '.t')).read_text().split('¬')[0])
            valid.append(source)
        valid += [ROOT / 'examples' / name / (name + '.n') for name in ['dungeon', 'pixels']]
        valid.append(ROOT / 'examples/text-adventure/quill.n')
        for source in valid:
            for flags in [(), ('-r',), ('--cgen',)]:
                reference = run(source, 1, flags)
                assert reference[0] == 0, reference[2]
                def check_runtime():
                    if source not in list(inputs.values()) + runtime_cases:
                        return
                    if '--cgen' in flags:
                        # Clang is only a compatibility-output test driver.
                        opts = subprocess.run([str(nerd), 'build', '--copts', str(source)],
                                              env=env, capture_output=True, text=True, check=True)
                        subprocess.run(['clang', '-Werror', str(output.with_suffix('.c')),
                                        *shlex.split(opts.stdout), '-o', str(output)],
                                       check=True, timeout=60)
                    subprocess.run([str(output)], check=True, timeout=10)

                check_runtime()
                for jobs in [2, 4, 8, 'auto']:
                    actual = run(source, jobs, flags)
                    assert actual[:5] == reference[:5], (source, flags, jobs, actual[2])
                    check_runtime()
                    # Successful inputs must exercise discovery/check separation,
                    # not silently obtain parity through the serial retry.
                    parse_end = max(r['start_ns'] + r['wall_ns'] for r in actual[5]
                                    if r.get('phase') == 'parse tokens into AST')
                    check_start = min(r['start_ns'] for r in actual[5]
                                      if r.get('phase') == 'analyse AST semantics')
                    assert check_start >= parse_end, (source, 'unexpected serial retry')
                    # Explicit file-stage edges must publish tokens before any
                    # parse starts, including re-parsing expanded folder modules.
                    modules = {r['module'] for r in actual[5]
                               if r.get('phase') == 'parse tokens into AST'}
                    for module in modules:
                        lexed = sorted(r['start_ns'] + r['wall_ns'] for r in actual[5]
                                       if r.get('module') == module and r.get('phase') == 'tokenise source text')
                        parsed = sorted(r['start_ns'] for r in actual[5]
                                        if r.get('module') == module and r.get('phase') == 'parse tokens into AST')
                        assert len(lexed) == len(parsed) and all(a <= b for a, b in zip(lexed, parsed)), module

            print('[PASS] front-end output identity:', source.parent.name, source.name, flush=True)

        failures = [
            ('semantic-before-parse', {'main.n': 'a :: use a\nb :: use b\nmain :: fn () {}\n',
                                       'a.n': 'pub bad :: unknown\n', 'b.n': 'pub broken :: fn (\n'}),
            ('cycle', {'main.n': 'a :: use a\nmain :: fn () {}\n',
                       'a.n': 'b :: use b\n', 'b.n': 'a :: use a\n'}),
            ('missing', {'main.n': 'absent :: use absent\nmain :: fn () {}\n'}),
            ('private-method', {'main.n': 'a :: use a\nmain :: fn () { value := a.Item {} value.secret() }\n',
                                'a.n': 'pub Item :: plex {}\nsecret :: fn (self: ^Item) {}\n'}),
            ('assert', {'main.n': 'on "never_enabled"\nmain :: fn () {}\n'}),
        ]
        for name, files in failures:
            source = case(name, files)
            reference = run(source, 1)
            assert reference[0] != 0, name
            for jobs in [2, 4, 8, 256, 'auto']:
                actual = run(source, jobs)
                assert actual[:3] == reference[:3], (name, jobs, actual[2], reference[2])
                assert actual[4] == reference[4], (name, jobs, 'profile labels')
            print('[PASS] ordered diagnostics and cleanup:', name, flush=True)

        # Without a shared implicit core, independent semantic closures can run
        # together. Vary body sizes and job counts to perturb completion order.
        rng = random.Random(5127)
        observed = set()
        for iteration in range(6):
            for index in range(12):
                count = rng.randrange(20, 90)
                (inputs['wide'].parent / f'd{index}.n').write_text(
                    'pub id :: fn [T] (value: T) -> T { return value }\n')
                argument, expected = ('yes', 7) if index % 2 else ('no', 9)
                (inputs['wide'].parent / f'm{index}.n').write_text(
                    f'use d{index}\n' +
                    'choose :: fn (enabled :: bool) -> i32 { return on enabled => 7 else 9 }\n' +
                    '\n'.join(f'f{i} :: fn (x: i32) -> i32 {{ return x + {i} }}'
                              for i in range(count)) +
                    f'\npub value :: fn (x: i32) -> i32 {{ return id[i32](f0(x)) + choose({argument}) - {expected} }}\n')
            reference = run(inputs['wide'], 1, library='')
            assert reference[0] == 0, reference[2]
            actual = run(inputs['wide'], rng.choice([2, 4, 8, 16]), library='')
            assert actual[:5] == reference[:5], ('randomized independent modules', actual[2])
            subprocess.run([str(output)], check=True, timeout=10)
            for phase in ['tokenise source text', 'parse tokens into AST', 'analyse AST semantics', 'generate HIR from sema']:
                if overlap(actual[5], phase):
                    observed.add(phase)
        assert 'analyse AST semantics' in observed and 'parse tokens into AST' in observed, observed
        print('[PASS] randomized completion; overlapping phases:', sorted(observed), flush=True)
    print('Scheduled front-end graph, diagnostics, FFI, generics and runtime checks passed')


if __name__ == '__main__':
    main()
