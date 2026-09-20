#!/usr/bin/env python3
"""Compare LLVM render worker counts, keeping paths and output bytes identical."""
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
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', nargs='+', type=int, default=[1, 2, 4, 8, 16])
    parser.add_argument('--cpus', nargs='+', type=int)
    parser.add_argument('--samples', type=int, default=5)
    parser.add_argument('--scenarios', nargs='+', default=['tiny', 'dungeon', 'pixels', 'quill', 'wide', 'deep', 'large'])
    args = parser.parse_args()
    if args.samples < 1 or any(j < 1 or j > 256 for j in args.jobs) or 1 not in args.jobs:
        parser.error('positive samples and worker counts 1-256 including 1 are required')
    if args.cpus:
        if not hasattr(os, 'sched_setaffinity'):
            parser.error('--cpus requires Linux affinity support')
        os.sched_setaffinity(0, set(args.cpus))
    nerd = args.nerd.resolve()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_DEBUG_KEEP_LINK_LLVM='1')
    for name in ['NERD_PROFILE', 'NERD_MEMORY_PROFILE', 'NERD_DEBUG_LLVM_SIDECARS']:
        env.pop(name, None)
    results = {'metadata': {
        'commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
        'dirty': bool(subprocess.check_output(['git', 'diff', 'HEAD'], cwd=ROOT)),
        'compiler': str(nerd), 'compiler_sha256': digest(nerd), 'platform': platform.platform(),
        'affinity': sorted(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else None,
        'jobs': args.jobs, 'samples': args.samples, 'warmups_per_jobs': 1,
        'order': 'rotate counts each iteration', 'keep_combined_llvm': True,
    }, 'runs': []}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='nerd-jobs-bench-') as directory:
        work = Path(directory)
        inputs = generate_inputs(work)
        inputs.update(dungeon=ROOT / 'examples/dungeon/dungeon.n',
                      pixels=ROOT / 'examples/pixels/pixels.n',
                      quill=ROOT / 'examples/text-adventure/quill.n')
        if set(args.scenarios) - inputs.keys():
            parser.error('unknown scenario')
        for scenario in args.scenarios:
            for target in ['debug', 'release']:
                binary = work / 'program.exe'
                ir = Path(str(binary) + '.link.ll')
                tail = ['build', *(['-r'] if target == 'release' else []),
                        '-o', str(binary), str(inputs[scenario])]
                row = {'scenario': scenario, 'target': target, 'arguments': tail,
                       'jobs': {str(j): {'samples': []} for j in args.jobs}}
                reference = None
                for iteration in range(args.samples + 1):
                    offset = iteration % len(args.jobs)
                    for jobs in args.jobs[offset:] + args.jobs[:offset]:
                        sample, _ = invoke([str(nerd), *tail, '--jobs', str(jobs)], env)
                        current = digest(ir)
                        if reference is None:
                            reference = current
                        assert current == reference, (scenario, target, jobs)
                        if iteration:
                            row['jobs'][str(jobs)]['samples'].append(sample)
                for jobs in args.jobs:
                    sample, records = invoke([str(nerd), *tail, '--jobs', str(jobs)],
                                             dict(env, NERD_PROFILE='1'))
                    assert digest(ir) == reference
                    renders = [r for r in records if r.get('phase') == 'render module LLVM']
                    # An elapsed envelope, not a sum of overlapping task times.
                    span = max(r['start_ns'] + r['wall_ns'] for r in renders) - min(r['start_ns'] for r in renders)
                    row['jobs'][str(jobs)].update(profile_sample=sample, profile=records,
                        render_span_ns=span, median_wall_ns=statistics.median(
                            s['wall_ns'] for s in row['jobs'][str(jobs)]['samples']))
                row.update(combined_sha256=reference, combined_bytes=ir.stat().st_size)
                results['runs'].append(row)
                args.output.write_text(json.dumps(results, indent=2) + '\n')
                print(f'{scenario:8} {target:7}: ' + ', '.join(
                    f'j{j}={row["jobs"][str(j)]["median_wall_ns"]/1e6:.2f}ms' for j in args.jobs), flush=True)


if __name__ == '__main__':
    main()
