#!/usr/bin/env python3
"""Repeatable serial compiler benchmarks. No generated program is executed."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parent.parent
PREFIX = 'nerd-profile\t'


def generate_inputs(directory, modules=16, functions=48):
    inputs = {}
    for shape in ['tiny', 'wide', 'deep', 'large']:
        folder = directory / shape
        folder.mkdir(parents=True)
        source = folder / 'main.n'
        if shape == 'tiny':
            source.write_text('main :: fn () -> i32 { return 0 }\n')
        elif shape == 'large':
            source.write_text('\n'.join(
                f'f{i} :: fn (x: i32) -> i32 {{ return x + {i} }}'
                for i in range(modules * functions)) +
                '\nmain :: fn () -> i32 { return f0(0) }\n')
        else:
            for i in range(modules):
                body = '\n'.join(
                    f'f{j} :: fn (x: i32) -> i32 {{ return x + {j} }}'
                    for j in range(functions))
                if shape == 'deep' and i + 1 < modules:
                    body = f'm{i+1} :: use m{i+1}\n' + body
                    value = f'm{i+1}.value(x)'
                else:
                    value = 'f0(x)'
                (folder / f'm{i}.n').write_text(body +
                    f'\npub value :: fn (x: i32) -> i32 {{ return {value} }}\n')
            imports = range(modules) if shape == 'wide' else range(1)
            source.write_text(''.join(f'm{i} :: use m{i}\n' for i in imports) +
                'main :: fn () -> i32 { return m0.value(0) }\n')
        inputs[shape] = source
    return inputs


def invoke(command, env):
    # Files avoid pipe deadlocks while wait4 obtains per-invocation accounting.
    with tempfile.TemporaryFile() as stdout, tempfile.TemporaryFile() as stderr:
        start = time.perf_counter_ns()
        process = subprocess.Popen(command, cwd=ROOT, env=env,
                                   stdout=stdout, stderr=stderr)
        usage = None
        if hasattr(os, 'wait4'):
            _, status, usage = os.wait4(process.pid, 0)
            process.returncode = os.waitstatus_to_exitcode(status)
        else:
            process.wait()
        elapsed = time.perf_counter_ns() - start
        stdout.seek(0)
        stderr.seek(0)
        output = stdout.read().decode('utf-8', errors='replace')
        errors = stderr.read().decode('utf-8', errors='replace')
    if process.returncode:
        raise RuntimeError(f'{command}\n{output}\n{errors}')
    sample = {'wall_ns': elapsed, 'user_seconds': None, 'system_seconds': None,
              'max_process_rss_bytes': None}
    if usage:
        sample.update(user_seconds=usage.ru_utime, system_seconds=usage.ru_stime,
                      max_process_rss_bytes=int(usage.ru_maxrss *
                          (1 if platform.system() == 'Darwin' else 1024)))
    return sample, [json.loads(line[len(PREFIX):]) for line in errors.splitlines()
                    if line.startswith(PREFIX)]


def evict_inputs(paths):
    if not hasattr(os, 'posix_fadvise'):
        raise RuntimeError('cold-inputs requires POSIX fadvise support')
    for path in paths:
        with path.open('rb') as source:
            os.posix_fadvise(source.fileno(), 0, 0, os.POSIX_FADV_DONTNEED)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=5)
    parser.add_argument('--warmups', type=int, default=1)
    parser.add_argument('--scenarios', nargs='+', default=['tiny', 'dungeon', 'pixels', 'quill', 'wide', 'deep', 'large'])
    parser.add_argument('--modes', nargs='+', choices=['check', 'llvm', 'cgen'], default=['check', 'llvm', 'cgen'])
    parser.add_argument('--targets', nargs='+', choices=['debug', 'release'], default=['debug', 'release'])
    parser.add_argument('--cache', choices=['warm', 'cold-inputs'], default='warm')
    parser.add_argument('--cpu', type=int, help='Pin compiler and its tools to one CPU (Linux)')
    args = parser.parse_args()
    if args.samples < 1 or args.warmups < 0:
        parser.error('samples must be positive and warmups nonnegative')
    if args.cpu is not None:
        if not hasattr(os, 'sched_setaffinity'):
            parser.error('--cpu requires Linux affinity support')
        os.sched_setaffinity(0, {args.cpu})
    nerd = args.nerd.resolve()
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))
    env.pop('NERD_PROFILE', None)
    env.pop('NERD_PROFILE_LOCKS', None)
    env.pop('NERD_MEMORY_PROFILE', None)
    env.pop('NERD_DEBUG_LLVM_SIDECARS', None)
    env.pop('NERD_DEBUG_KEEP_LINK_LLVM', None)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    metadata = {
        'schema': 1, 'commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
        'dirty': bool(subprocess.check_output(['git', 'diff', 'HEAD'], cwd=ROOT)),
        'compiler': str(nerd), 'compiler_sha256': hashlib.sha256(nerd.read_bytes()).hexdigest(),
        'platform': platform.platform(), 'cpu': platform.processor(), 'logical_cpus': os.cpu_count(),
        'affinity': sorted(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else None,
        'cache': args.cache, 'samples': args.samples, 'warmups': args.warmups,
        'synthetic_modules': 16, 'synthetic_functions_per_module': 48,
        'tool_versions': {},
    }
    for tool in ['opt', 'llc', 'ld.lld', 'lld-link', 'ld64.lld']:
        try:
            result = subprocess.run([tool, '--version'], capture_output=True, text=True)
            metadata['tool_versions'][tool] = (result.stdout + result.stderr).strip()
        except FileNotFoundError:
            pass
    results = {'metadata': metadata, 'runs': []}
    with tempfile.TemporaryDirectory(prefix='nerd-benchmark-') as temp:
        work = Path(temp)
        inputs = generate_inputs(work)
        inputs.update(dungeon=ROOT / 'examples/dungeon/dungeon.n',
                      pixels=ROOT / 'examples/pixels/pixels.n',
                      quill=ROOT / 'examples/text-adventure/quill.n')
        unknown = set(args.scenarios) - inputs.keys()
        if unknown:
            parser.error(f'Unknown scenarios: {sorted(unknown)}')
        source_paths = list(work.rglob('*.n')) + list((ROOT / 'mods').rglob('*.n')) + list((ROOT / 'examples').rglob('*.n'))
        metadata['source_sha256'] = {str(p.relative_to(work) if p.is_relative_to(work) else p.relative_to(ROOT)):
            hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
        for name in args.scenarios:
            for mode in args.modes:
                for target in args.targets:
                    output = work / ('output.c' if mode == 'cgen' else 'output.exe')
                    command = [str(nerd), 'check' if mode == 'check' else 'build']
                    if target == 'release':
                        command.append('-r')
                    if mode == 'cgen':
                        command.append('--cgen')
                    if mode != 'check':
                        command += ['-o', str(output)]
                    command.append(str(inputs[name]))
                    samples = []
                    for i in range(args.warmups + args.samples):
                        if args.cache == 'cold-inputs':
                            evict_inputs(source_paths)
                        sample, _ = invoke(command, env)
                        if i >= args.warmups:
                            samples.append(sample)
                    if args.cache == 'cold-inputs':
                        evict_inputs(source_paths)
                    profile_sample, records = invoke(command, dict(env, NERD_PROFILE='1'))
                    if not records:
                        raise RuntimeError('Compiler produced no profiling records; rebuild it first')
                    results['runs'].append({
                        'scenario': name, 'mode': mode, 'target': target, 'command': command,
                        'samples': samples, 'median_wall_ns': statistics.median(s['wall_ns'] for s in samples),
                        'profile_sample': profile_sample, 'profile': records,
                        'artifact_bytes': output.stat().st_size if mode != 'check' else 0,
                    })
                    args.output.write_text(json.dumps(results, indent=2) + '\n')
                    print(f'{name:8} {mode:5} {target:7}: {results["runs"][-1]["median_wall_ns"]/1e6:.2f} ms', flush=True)


if __name__ == '__main__':
    main()
