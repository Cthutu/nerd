#!/usr/bin/env python3
"""Compare an uninstrumented main compiler with the scheduling experiment."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import statistics
import subprocess
import tempfile

from benchmark_compiler import ROOT, generate_inputs, invoke


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def phases(output):
    result = {}
    text = re.sub(r'\x1b\[[0-9;]*m', '', output)
    units = {'ns': 1, 'μs': 1000, 'µs': 1000, 'us': 1000, 'ms': 1000000, 's': 1000000000}
    for line in text.splitlines():
        cells = [part.strip() for part in line.split('│')]
        if len(cells) != 5 or cells[1] not in ['front-end', 'back-end'] or cells[2].endswith(' total'):
            continue
        match = re.fullmatch(r'([\d.]+)\s+(\S+)', cells[3])
        if match is None or match[2] not in units:
            raise ValueError('Unknown timing duration: ' + cells[3])
        key = cells[1] + '/' + cells[2]
        result[key] = result.get(key, 0) + float(match[1]) * units[match[2]]
    if not result:
        raise ValueError('No phase timings in --timing output')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before', type=Path, required=True)
    parser.add_argument('--after', type=Path, required=True)
    parser.add_argument('--before-ref', default='main')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cpus', type=int, nargs='+', required=True)
    parser.add_argument('--after-jobs', type=int, nargs='+', default=[1])
    parser.add_argument('--samples', type=int, default=5)
    parser.add_argument('--modes', nargs='+', choices=['check', 'debug', 'release'], default=['check', 'debug', 'release'])
    parser.add_argument('--scenarios', nargs='+', default=['tiny', 'dungeon', 'pixels', 'quill', 'wide', 'deep', 'large'])
    args = parser.parse_args()
    if args.samples < 1 or any(j < 1 or j > 256 for j in args.after_jobs):
        parser.error('positive samples and job counts 1-256 required')
    os.sched_setaffinity(0, set(args.cpus))
    before, after = args.before.resolve(), args.after.resolve()
    versions = {'main': (before, None)}
    versions.update({f'experiment-j{j}': (after, j) for j in args.after_jobs})
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_DEBUG_KEEP_LINK_LLVM='1')
    for key in ['NERD_PROFILE', 'NERD_PROFILE_LOCKS', 'NERD_MEMORY_PROFILE', 'NERD_DEBUG_LLVM_SIDECARS']:
        env.pop(key, None)
    results = {'metadata': {
        'baseline_commit': subprocess.check_output(['git', 'rev-parse', args.before_ref], cwd=ROOT, text=True).strip(),
        'experiment_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
        'platform': platform.platform(), 'affinity': sorted(os.sched_getaffinity(0)),
        'compilers': {name: {'path': str(path), 'sha256': digest(path), 'jobs': jobs}
                      for name, (path, jobs) in versions.items()},
        'samples': args.samples, 'warmups': 1, 'order': 'rotating variants each iteration',
        'inputs': 'identical source, output paths and library; warm filesystem cache',
        'timing_tables': 'one separate --timing invocation per cell; not included in latency samples',
        'backend_difference': 'main invokes clang; experiment invokes opt/llc/LLVM linker directly',
    }, 'runs': []}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='nerd-main-compare-') as directory:
        work = Path(directory)
        inputs = generate_inputs(work / 'sources')
        synthetic = set(inputs)
        inputs.update(dungeon=ROOT / 'examples/dungeon/dungeon.n',
                      pixels=ROOT / 'examples/pixels/pixels.n',
                      quill=ROOT / 'examples/text-adventure/quill.n')
        for scenario in args.scenarios:
            for mode in args.modes:
                names = list(versions)
                # check has no --jobs option; compare only once.
                if mode == 'check':
                    names = ['main', next(n for n in versions if n != 'main')]
                output = work / 'program'
                ir = Path(str(output) + '.link.ll')
                commands = {}
                for name in names:
                    compiler, jobs = versions[name]
                    commands[name] = [str(compiler), 'check' if mode == 'check' else 'build',
                                      *(['--jobs', str(jobs)] if jobs is not None and mode != 'check' else []),
                                      *(['-r'] if mode == 'release' else []),
                                      str(inputs[scenario]),
                                      *([] if mode == 'check' else ['-o', str(output)])]
                row = {'scenario': scenario, 'mode': mode,
                       'variants': {name: {'command': commands[name], 'samples': []} for name in names}}
                for iteration in range(args.samples + 1):
                    offset = iteration % len(names)
                    for name in names[offset:] + names[:offset]:
                        sample, _ = invoke(commands[name], env)
                        cell = row['variants'][name]
                        if mode != 'check':
                            current = digest(ir)
                            if 'llvm_sha256' in cell:
                                assert current == cell['llvm_sha256'], (scenario, mode, name, 'unstable LLVM')
                            cell['llvm_sha256'] = current
                            cell['llvm_bytes'] = ir.stat().st_size
                            if iteration == 0 and scenario in synthetic:
                                subprocess.run([str(output)], check=True, timeout=10)
                                cell['runtime_exit'] = 0
                        if iteration:
                            cell['samples'].append(sample)
                for name in names:
                    cell = row['variants'][name]
                    cell['median_wall_ns'] = statistics.median(s['wall_ns'] for s in cell['samples'])
                    command = commands[name]
                    result = subprocess.run([command[0], '--timing', *command[1:]], cwd=ROOT,
                                            env=env, text=True, capture_output=True, timeout=180)
                    assert result.returncode == 0, result.stderr
                    cell['timing_table'] = result.stdout + result.stderr
                    cell['phase_sum_ns'] = phases(cell['timing_table'])
                if mode != 'check':
                    row['llvm_identical'] = len({c['llvm_sha256'] for c in row['variants'].values()}) == 1
                    assert len({c['llvm_sha256'] for n, c in row['variants'].items() if n != 'main'}) == 1
                results['runs'].append(row)
                args.output.write_text(json.dumps(results, indent=2) + '\n')
                print(f'{scenario:8} {mode:7}: ' + ', '.join(
                    f'{name}={row["variants"][name]["median_wall_ns"]/1e6:.2f}ms' for name in names) +
                    (f'; LLVM identical={row["llvm_identical"]}' if mode != 'check' else ''), flush=True)


if __name__ == '__main__':
    main()
