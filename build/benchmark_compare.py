#!/usr/bin/env python3
"""Alternate two compilers on identical inputs and verify combined LLVM bytes."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import tempfile

from benchmark_compiler import ROOT, generate_inputs, invoke


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before', type=Path, required=True)
    parser.add_argument('--after', type=Path, default=ROOT / '_bin/nerd')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=5)
    parser.add_argument('--cpu', type=int)
    parser.add_argument('--cpus', nargs='+', type=int)
    parser.add_argument('--jobs', help='Pass 1-256 or auto to both compilers')
    parser.add_argument('--profile-samples', type=int, default=1,
                        help='Separate instrumented samples for source-to-IR spans')
    parser.add_argument('--scenarios', nargs='+', default=['tiny', 'dungeon', 'pixels', 'quill', 'wide', 'deep', 'large'])
    args = parser.parse_args()
    if args.samples < 1 or args.profile_samples < 1:
        parser.error('sample counts must be positive')
    if args.cpu is not None and args.cpus:
        parser.error('choose --cpu or --cpus')
    if args.jobs is not None and args.jobs != 'auto' and (not args.jobs.isdecimal() or not 1 <= int(args.jobs) <= 256):
        parser.error('--jobs must be 1-256 or auto')
    if args.cpu is not None or args.cpus:
        if not hasattr(os, 'sched_setaffinity'):
            parser.error('CPU affinity requires Linux support')
        os.sched_setaffinity(0, set(args.cpus) if args.cpus else {args.cpu})
    compilers = {'before': args.before.resolve(), 'after': args.after.resolve()}
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_DEBUG_KEEP_LINK_LLVM='1')
    for name in ['NERD_PROFILE', 'NERD_PROFILE_LOCKS', 'NERD_MEMORY_PROFILE', 'NERD_DEBUG_LLVM_SIDECARS']:
        env.pop(name, None)
    results = {'metadata': {
        'commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
        'dirty': bool(subprocess.check_output(['git', 'diff', 'HEAD'], cwd=ROOT)),
        'platform': platform.platform(),
        'affinity': sorted(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else None,
        'compilers': {key: {'path': str(path), 'sha256': digest(path)} for key, path in compilers.items()},
        'samples': args.samples, 'profile_samples': args.profile_samples, 'warmups_per_compiler': 1, 'order': 'alternating',
        'keep_combined_llvm': True, 'jobs': args.jobs,
    }, 'runs': []}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='nerd-compare-') as directory:
        work = Path(directory)
        inputs = generate_inputs(work)
        inputs.update(dungeon=ROOT / 'examples/dungeon/dungeon.n',
                      pixels=ROOT / 'examples/pixels/pixels.n',
                      quill=ROOT / 'examples/text-adventure/quill.n')
        if set(args.scenarios) - inputs.keys():
            parser.error('Unknown scenario')
        for scenario in args.scenarios:
            for target in ['debug', 'release']:
                binary = work / 'program.exe'
                ir = Path(str(binary) + '.link.ll')
                tail = ['build', *(['--jobs', str(args.jobs)] if args.jobs is not None else []),
                        *(['-r'] if target == 'release' else []),
                        '-o', str(binary), str(inputs[scenario])]
                row = {'scenario': scenario, 'target': target, 'arguments': tail,
                       'before': {'samples': []}, 'after': {'samples': []}}
                reference = None
                for iteration in range(args.samples + 1):
                    for key in (['before', 'after'] if iteration % 2 == 0 else ['after', 'before']):
                        sample, _ = invoke([str(compilers[key]), *tail], env)
                        current = digest(ir)
                        if reference is None:
                            reference = current
                        assert current == reference, f'Combined LLVM changed: {scenario}/{target}/{key}'
                        if iteration:
                            row[key]['samples'].append(sample)
                for key in compilers:
                    profiles = []
                    for _ in range(args.profile_samples):
                        sample, records = invoke([str(compilers[key]), *tail], dict(env, NERD_PROFILE='1'))
                        assert records, 'Both compilers must support NERD_PROFILE'
                        assert digest(ir) == reference
                        phases = [r for r in records if r.get('kind') == 'phase']
                        front = [r for r in phases if r.get('stage') == 'front-end']
                        first = min(r['start_ns'] for r in front)
                        combined = [r for r in phases if r.get('phase') == 'combine LLVM text']
                        profiles.append(dict(sample=sample, records=records,
                            front_span_ns=max(r['start_ns'] + r['wall_ns'] for r in front) - first,
                            source_to_ir_span_ns=max(r['start_ns'] + r['wall_ns'] for r in combined) - first))
                    row[key].update(profile_sample=sample, profile=records, profiles=profiles,
                                    median_source_to_ir_ns=statistics.median(p['source_to_ir_span_ns'] for p in profiles),
                                    median_front_span_ns=statistics.median(p['front_span_ns'] for p in profiles),
                                    median_wall_ns=statistics.median(x['wall_ns'] for x in row[key]['samples']))
                row.update(combined_sha256=reference, combined_bytes=ir.stat().st_size)
                results['runs'].append(row)
                args.output.write_text(json.dumps(results, indent=2) + '\n')
                print(f'{scenario:8} {target:7}: {row["before"]["median_wall_ns"]/1e6:.2f} -> '
                      f'{row["after"]["median_wall_ns"]/1e6:.2f} ms; LLVM identical', flush=True)


if __name__ == '__main__':
    main()
