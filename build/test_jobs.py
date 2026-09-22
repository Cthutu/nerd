#!/usr/bin/env python3
"""Exercise the production LLVM scheduler, ordered output and failure cleanup."""
import argparse
from contextlib import contextmanager
import json
import os
from pathlib import Path
import subprocess
import tempfile

from benchmark_compiler import generate_inputs
from test_render_threads import sanitized_compiler

ROOT = Path(__file__).resolve().parents[1]
PREFIX = 'nerd-profile\t'


@contextmanager
def restricted_cpus(count):
    """Restrict the test process so children inherit a known CPU budget."""
    if hasattr(os, 'sched_getaffinity'):
        original = os.sched_getaffinity(0)
        chosen = sorted(original)[-count:]
        try:
            os.sched_setaffinity(0, chosen)
            yield len(chosen)
        finally:
            os.sched_setaffinity(0, original)
    elif os.name == 'nt':
        import ctypes
        from ctypes import wintypes
        kernel = ctypes.WinDLL('kernel32', use_last_error=True)
        kernel.GetCurrentProcess.restype = wintypes.HANDLE
        kernel.GetProcessAffinityMask.argtypes = [wintypes.HANDLE,
                                                 ctypes.POINTER(ctypes.c_size_t),
                                                 ctypes.POINTER(ctypes.c_size_t)]
        kernel.SetProcessAffinityMask.argtypes = [wintypes.HANDLE, ctypes.c_size_t]
        process = kernel.GetCurrentProcess()
        original, system = ctypes.c_size_t(), ctypes.c_size_t()
        assert kernel.GetProcessAffinityMask(process, ctypes.byref(original), ctypes.byref(system))
        bits = [i for i in range(ctypes.sizeof(original) * 8) if original.value & (1 << i)]
        chosen = bits[-count:]
        assert chosen, 'No primary-group CPU affinity available'
        try:
            assert kernel.SetProcessAffinityMask(process, sum(1 << i for i in chosen))
            yield len(chosen)
        finally:
            assert kernel.SetProcessAffinityMask(process, original.value)
    else:
        yield None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', type=Path, default=ROOT / '_bin/nerd-debug')
    parser.add_argument('--sanitize', choices=['thread', 'address'])
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='nerd-jobs-') as directory:
        work = Path(directory)
        nerd = (sanitized_compiler(work / 'compiler', args.sanitize)
                if args.sanitize else args.nerd.resolve())
        env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'), NERD_PROFILE='1',
                   NERD_DEBUG_KEEP_LINK_LLVM='1', COLUMNS='32768')
        env.pop('NERD_MEMORY_PROFILE', None)
        env.pop('NERD_PROFILE_LOCKS', None)
        env.pop('NERD_DEBUG_LLVM_SIDECARS', None)

        def run(*flags, success=True):
            result = subprocess.run([str(nerd), 'build', *map(str, flags)],
                                    cwd=work, env=env, capture_output=True,
                                    text=True, timeout=180)
            assert (result.returncode == 0) == success, result.stderr
            records = [json.loads(line[len(PREFIX):]) for line in result.stderr.splitlines()
                       if line.startswith(PREFIX)]
            return result, records

        def labels(records):
            return [(r.get('kind'), r.get('stage'), r.get('phase'), r.get('module'),
                     r.get('dependency'), r.get('output_bytes'), r.get('success'))
                    for r in records if r.get('kind') not in ('scheduler', 'scheduler-policy')]

        inputs = generate_inputs(work / 'inputs', modules=6, functions=8)
        # Grow function scratch beyond its initial commitment, then reuse it
        # for a much smaller function. Runtime and debug-IR parity catch stale
        # text/metadata or function state surviving a reset.
        growth = work / 'scratch-growth.n'
        growth.write_text('seed: i32 = 7\n'
                          'large :: fn (x: i32) -> i32 {\n value := x\n' +
                          ' value += 1\n' * 1500 + ' return value\n}\n'
                          'small :: fn () -> i32 { return seed }\n'
                          'main :: fn () -> i32 { return large(0) + small() - 1507 }\n')
        inputs['scratch-growth'] = growth
        # The same source/output paths make byte comparisons meaningful even
        # for debug metadata. Each compiled synthetic program returns zero.
        for name, source in inputs.items():
            output = work / 'program.exe'
            for release in [False, True]:
                flags = ['--llvm', source, '-o', output] + (['-r'] if release else [])
                _, serial = run(*flags)
                reference = {p.name: p.read_bytes() for p in work.glob('*.ll')}
                assert reference and any(p.endswith('.link.ll') for p in reference)
                for jobs in [1, 2, 4, 8, 256]:
                    _, parallel = run('--jobs', jobs, *flags)
                    assert labels(serial) == labels(parallel), (name, release, jobs)
                    batches = [r for r in parallel if r['kind'] == 'scheduler']
                    assert len(batches) == (jobs > 1)
                    if batches:
                        batch = batches[0]
                        assert batch['completed'] == batch['modules'] and batch['success']
                        assert batch['slots'] == min(jobs, batch['modules'])
                        assert batch['first_dispatch_ns'] + batch['drain_ns'] <= batch['wall_ns']
                        assert batch['dispatch_delay_ns_sum'] >= batch['first_dispatch_ns']

                    assert reference == {p.name: p.read_bytes() for p in work.glob('*.ll')}, (name, jobs)
                    subprocess.run([str(output)], check=True, timeout=10)
            print(f'[PASS] --jobs LLVM identity and execution: {name}', flush=True)

        source = inputs['wide']
        output = work / 'auto.exe'
        flags = ['--llvm', source, '-o', output]
        _, serial = run('--jobs', 1, *flags)
        reference = {p.name: p.read_bytes() for p in work.glob('*.ll')}
        _, automatic = run('--jobs', 'auto', *flags)
        assert labels(automatic) == labels(serial)
        assert reference == {p.name: p.read_bytes() for p in work.glob('*.ll')}
        subprocess.run([str(output)], check=True, timeout=10)
        for count in [1, 2, 3, 4, 8]:
            with restricted_cpus(count) as available:
                if available is None:
                    print('[SKIP] process-affinity auto sizing on this platform', flush=True)
                    break
                for requested, expected in [('auto', max(1, available // 2)), (1, 1), (4, 4)]:
                    _, records = run('--jobs', requested, *flags)
                    if requested == 'auto':
                        policy = [r for r in records if r['kind'] == 'scheduler-policy'
                                  and r['phase'] == 'LLVM render']
                        assert len(policy) == 1 and policy[0]['ceiling'] == expected
                        assert 1 <= policy[0]['jobs'] <= expected
                        expected = policy[0]['jobs']
                    batches = [r for r in records if r['kind'] == 'scheduler']
                    assert len(batches) == (expected > 1), (available, requested, batches)
                    if batches:
                        assert batches[0]['jobs'] == expected, (available, requested, batches)
                        assert batches[0]['slots'] == min(expected, batches[0]['modules'])
                    assert reference == {p.name: p.read_bytes() for p in work.glob('*.ll')}
                    subprocess.run([str(output)], check=True, timeout=10)
        # Small/dominated batches should not create a worker pool even when
        # the CPU ceiling allows it. Numeric overrides above still do.
        _, tiny_auto = run('--jobs', 'auto', inputs['tiny'], '-o', work / 'tiny-auto.exe')
        policies = [r for r in tiny_auto if r['kind'] == 'scheduler-policy']
        assert policies and all(r['jobs'] == 1 for r in policies), policies
        assert not any(r['kind'] == 'scheduler' for r in tiny_auto)
        _, default = run(*flags)
        assert not any(r['kind'] == 'scheduler' for r in default), 'Default changed before adoption gate'
        run('--cgen', '--jobs', 1, source, '-o', work / 'auto.c')
        serial_c = (work / 'auto.c').read_bytes()
        run('--cgen', '--jobs', 'auto', source, '-o', work / 'auto.c')
        assert (work / 'auto.c').read_bytes() == serial_c
        print('[PASS] automatic half-CPU sizing, affinity, numeric overrides and LLVM/C parity', flush=True)

        # Lock timing is opt-in and must not change generated code or labels.
        output = work / 'locks.exe'
        _, ordinary = run('--jobs', 4, source, '-o', output)
        reference = Path(str(output) + '.link.ll').read_bytes()
        assert all('memory_lock_acquisitions' not in r for r in ordinary)
        env['NERD_PROFILE_LOCKS'] = '1'
        for jobs in [1, 4]:
            _, measured = run('--jobs', jobs, source, '-o', output)
            assert labels(ordinary) == labels(measured)
            renders = [r for r in measured if r.get('phase') == 'render module LLVM']
            assert renders and all(r['memory_lock_acquisitions'] > 0 for r in renders)
            assert all(r['memory_lock_acquire_ns'] >= 0 for r in renders)
            assert Path(str(output) + '.link.ll').read_bytes() == reference
        del env['NERD_PROFILE_LOCKS']
        run('-j', 4, source, '-o', work / 'short.exe')
        for value in ['0', '-1', '257', 'abc', '1.5', '', '999999999999999999999']:
            result, records = run('--jobs', value, source, success=False)
            assert '--jobs' in result.stderr and not records, result.stderr
        run('--jobs', success=False)

        # All worker results must be joined before a coordinator output failure.
        output = work / 'blocked.exe'
        obstruction = Path(str(output) + '.m1.ll')
        obstruction.mkdir()
        result, records = run('--jobs', 4, '--llvm', source, '-o', output, success=False)
        assert str(obstruction) in result.stderr and 'internal compiler error' not in result.stderr, result.stderr
        assert not any(r.get('stage') == 'tool' for r in records)
        assert sum(r.get('phase') == 'render module LLVM' for r in records) == 2
        assert (work / '_blocked.ll').is_file()
        obstruction.rmdir()
        run('--jobs', 4, '--llvm', source, '-o', output)
        subprocess.run([str(output)], check=True, timeout=10)

        # Export rendering and object/archive/shared-library production use the
        # same scheduler; C compatibility output remains serial and unchanged.
        for kind in ['--obj', '--lib', '--dll']:
            output = work / ('artifact' + kind[2:])
            run('--jobs', 1, kind, source, '-o', output)
            serial = Path(str(output) + '.link.ll').read_bytes()
            run('--jobs', 4, kind, source, '-o', output)
            assert serial == Path(str(output) + '.link.ll').read_bytes()
        output = work / 'program.c'
        run('--jobs', 1, '--cgen', source, '-o', output)
        serial = output.read_bytes()
        run('--jobs', 4, '--cgen', source, '-o', output)
        assert output.read_bytes() == serial
        serial, _ = run('--jobs', 1, '--copts', source)
        parallel, _ = run('--jobs', 4, '--copts', source)
        assert serial.stdout == parallel.stdout
    print('LLVM scheduler CLI, ordered output, artifacts and failure recovery passed')


if __name__ == '__main__':
    main()
