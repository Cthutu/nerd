#!/usr/bin/env python3
"""Profiling contracts: observational output, labelled phases, graph and failures."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

from benchmark_compiler import generate_inputs

ROOT = Path(__file__).resolve().parent.parent
PREFIX = 'nerd-profile\t'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug')
    nerd = parser.parse_args().nerd.resolve()
    with tempfile.TemporaryDirectory(prefix='nerd-profile-') as directory:
        work = Path(directory)
        source = work / 'main.n'
        source.write_text('dep :: use dep\nmain :: fn () -> i32 { return dep.value() }\n')
        (work / 'dep.n').write_text('pub value :: fn () -> i32 { return 0 }\n')
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))

        def run(profile, *args, success=True):
            result = subprocess.run([str(nerd), *map(str, args)], cwd=work,
                                    env=dict(env, NERD_PROFILE=profile),
                                    capture_output=True, text=True)
            assert (result.returncode == 0) == success, result.stderr
            records = [json.loads(line[len(PREFIX):]) for line in result.stderr.splitlines()
                       if line.startswith(PREFIX)]
            return result, records

        _, disabled = run('0', 'check', source)
        assert not disabled
        _, records = run('1', 'check', source)
        phases = [r for r in records if r['kind'] == 'phase']
        assert all(r['stage'] == 'front-end' and r['success'] for r in phases)
        for name in ['main.n', 'dep.n']:
            assert any(r['module'].endswith(name) and r['phase'] == 'analyse AST semantics' for r in phases)
        assert any(r['kind'] == 'dependency' and r['module'].endswith('main.n')
                   and r['dependency'].endswith('dep.n') for r in records)
        for r in phases:
            assert r['wall_ns'] >= 0 and r['start_ns'] > 0
            assert r['cpu_ns'] is None or r['cpu_ns'] >= 0
            if os.name == 'nt' or os.sys.platform.startswith('linux'):
                assert r['cpu_ns'] is not None
            assert r['heap_peak_bytes'] >= r['heap_live_bytes']
        binary = work / 'program.exe'
        _, records = run('1', 'build', '-r', source, '-o', binary)
        tools = {r['phase'] for r in records if r.get('stage') == 'tool'}
        assert {'opt', 'llc'} <= tools
        assert tools & {'ld.lld', 'lld-link', 'ld64.lld'}
        assert any(r.get('phase') == 'render module LLVM' and r['output_bytes'] > 0 for r in records)
        assert any(r.get('phase') == 'combine LLVM text' and r['output_bytes'] > 0 for r in records)
        subprocess.run([str(binary)], check=True)
        _, sidecars = run('1', 'build', '--llvm', source, '-o', binary)
        llvm_files = {p.name: p.read_bytes() for p in work.glob('*.ll')}
        assert llvm_files
        assert any(r.get('phase') == 'render LLVM sidecar' for r in sidecars)
        run('0', 'build', '--llvm', source, '-o', binary)
        assert llvm_files == {p.name: p.read_bytes() for p in work.glob('*.ll')}
        output = work / 'program.c'
        _, c_records = run('1', 'build', '--cgen', source, '-o', output)
        profiled_c = output.read_bytes()
        run('0', 'build', '--cgen', source, '-o', output)
        assert output.read_bytes() == profiled_c
        assert any(r.get('phase') == 'render C' for r in c_records)
        plain, _ = run('0', 'build', '--copts', source)
        profiled, _ = run('1', 'build', '--copts', source)
        assert plain.stdout == profiled.stdout and len(plain.stdout.splitlines()) == 1
        for shape, generated in generate_inputs(work / 'synthetic', modules=3, functions=2).items():
            _, graph = run('1', 'check', generated)
            if shape == 'wide':
                assert sum(r['kind'] == 'dependency' and r['module'] == str(generated)
                           and r['dependency'].endswith(('m0.n', 'm1.n', 'm2.n')) for r in graph) == 3
            if shape == 'deep':
                assert any(r['kind'] == 'dependency' and r['module'].endswith('m1.n')
                           and r['dependency'].endswith('m2.n') for r in graph)
        bad = work / ('bad"name.n' if os.name != 'nt' else 'bad-name.n')
        bad.write_text('main :: fn () { undefined_function() }\n')
        _, failed = run('1', 'check', bad, success=False)
        assert any(r.get('success') is False and r['module'] == str(bad) for r in failed)
    print('Profiling labels, graph, CPU/memory counters, failures and output parity passed')


if __name__ == '__main__':
    main()
