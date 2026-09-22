#!/usr/bin/env python3
"""Collect native Windows validation evidence without installing Nerd globally."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import platform
import shutil
import signal
import subprocess
import sys
import time
import uuid

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


@dataclass
class Stage:
    name: str
    command: list[str]
    needs: tuple[str, ...] = ()


def plan(folder: Path) -> list[Stage]:
    python = sys.executable
    suffix = '.exe' if os.name == 'nt' else ''
    stages = [Stage('build-debug', [python, 'build/build.py', 'nerd', '--skip-mod-sync']),
              Stage('build-release', [python, 'build/build.py', '-r', 'nerd', '--skip-mod-sync'])]
    for name in ['clean', 'memory', 'threads']:
        stages.append(Stage(name, [python, f'build/test_{name}.py']))
    stages.append(Stage('fixtures', [python, 'build/test.py'], ('build-debug',)))
    for mode in ['debug', 'release']:
        nerd = str(ROOT / '_bin' / (('nerd-debug' if mode == 'debug' else 'nerd') + suffix))
        needs = ('build-' + mode,)
        stages.append(Stage('doctor-' + mode, [nerd, 'doctor'], needs))
        for name in ['profile', 'render_threads', 'expression_depth', 'jobs', 'front_threads', 'toolchain', 'install', 'cgen']:
            command = [python, f'build/test_{name}.py', '--nerd', nerd]
            if name == 'expression_depth' and mode == 'release':
                command += ['--terms', '6000']
            stages.append(Stage(name.replace('_', '-') + '-' + mode,
                                command, needs))
        stages.append(Stage('debugger-' + mode,
                            [python, 'build/check_debugger_stepping.py', '--nerd', nerd, '--jobs', '4'], needs))
        stages.append(Stage('editor-' + mode,
                            [python, 'build/check_editor_integrations.py', '--nerd', nerd], needs))
    stages.append(Stage('benchmark-accounting', [python, 'build/test_benchmark_usage.py']))
    stages.append(Stage('adapter-transforms', [python, 'build/check_debugger_adapter_transforms.py']))
    # Benchmark only after every selected correctness stage succeeds. Never run
    # latency measurements concurrently with compiler builds or other tests.
    needs = tuple(stage.name for stage in stages)
    stages.append(Stage('benchmark', [python, 'build/benchmark_jobs.py', '--nerd',
                                     str(ROOT / '_bin' / ('nerd' + suffix)),
                                     '--jobs', '1', '2', '4', '8', '16', 'auto', '--output',
                                     str(folder / 'benchmark.json')], needs))
    return stages


def select(stages: list[Stage], names: list[str] | None) -> list[Stage]:
    selected = set(names) if names else {s.name for s in stages}
    known = {s.name: s for s in stages}
    unknown = selected - known.keys()
    if unknown:
        raise ValueError('Unknown stages: ' + ', '.join(sorted(unknown)))
    pending = list(selected)
    while pending:
        for dependency in known[pending.pop()].needs:
            if dependency not in selected:
                selected.add(dependency)
                pending.append(dependency)
    return [s for s in stages if s.name in selected]


def execute(stage: Stage, folder: Path, env: dict[str, str], timeout: float,
            cwd: Path = ROOT) -> dict:
    result = {'name': stage.name, 'command': stage.command, 'cwd': str(cwd),
              'started_utc': datetime.now(timezone.utc).isoformat(),
              'log': stage.name + '.log', 'status': 'running'}
    start = time.monotonic()
    with (folder / result['log']).open('wb') as log:
        try:
            process = subprocess.Popen(stage.command, cwd=cwd, env=env,
                                       stdout=log, stderr=subprocess.STDOUT,
                                       start_new_session=os.name != 'nt')
            try:
                result['exit_code'] = process.wait(timeout=timeout)
                result['status'] = 'passed' if process.returncode == 0 else 'failed'
            except (subprocess.TimeoutExpired, KeyboardInterrupt) as error:
                # Kill descendants too: tests launch Nerd, LLVM and executables.
                result['status'] = 'timeout' if isinstance(error, subprocess.TimeoutExpired) else 'interrupted'
                try:
                    if os.name == 'nt':
                        killed = subprocess.run(['taskkill', '/PID', str(process.pid), '/T', '/F'],
                                                stdout=log, stderr=subprocess.STDOUT, timeout=30)
                        if killed.returncode and process.poll() is None:
                            raise RuntimeError('taskkill failed to terminate the process tree')
                    else:
                        try:
                            os.killpg(process.pid, signal.SIGKILL)
                        except ProcessLookupError:
                            pass
                except (OSError, subprocess.SubprocessError, RuntimeError) as cleanup_error:
                    result.update(status='cleanup-failed', error=str(cleanup_error))
                finally:
                    process.kill()
                    process.wait(timeout=10)
        except OSError as error:
            result.update(status='failed', error=str(error))
            log.write((str(error) + '\n').encode('utf-8'))
    result['seconds'] = round(time.monotonic() - start, 3)
    return result


def save(folder: Path, report: dict) -> None:
    temporary = folder / 'summary.tmp'
    temporary.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    temporary.replace(folder / 'summary.json')
    title = 'Windows validation run' if report.get('native_windows') else 'Non-Windows harness smoke test'
    lines = ['# ' + title, '', f"Platform: {report['platform']}",
             f"Commit at start: `{report['commit']}`", f"Branch: `{report['branch']}`", '',
             f"Selection: {report['selection']}",
             f"Automated status: **{report['status']}**. Manual checks: see MANUAL.md.", '',
             '| Stage | Status | Seconds | Log / reason |', '| --- | --- | ---: | --- |']
    for item in report['stages']:
        detail = f"[{item['log']}]({item['log']})" if 'log' in item else item.get('reason', '')
        lines.append(f"| {item['name']} | {item['status']} | {item.get('seconds', '')} | {detail} |")
    (folder / 'SUMMARY.md').write_text('\n'.join(lines) + '\n', encoding='utf-8')


def git(*args: str) -> str:
    try:
        return subprocess.check_output(['git', *args], cwd=ROOT, text=True,
                                       stderr=subprocess.STDOUT, timeout=15).strip()
    except (OSError, subprocess.SubprocessError) as error:
        return 'unavailable: ' + str(error)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--only', nargs='+', help='Run named stages and their prerequisites')
    parser.add_argument('--list', action='store_true', help='Print selected commands without running them')
    parser.add_argument('--timeout', type=float, default=3600, help='Seconds allowed per stage')
    parser.add_argument('--allow-non-windows', action='store_true', help='Harness smoke tests only; not Windows evidence')
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    folder = HERE / 'results' / (datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex[:8])
    try:
        stages = select(plan(folder), args.only)
    except ValueError as error:
        parser.error(str(error))
    if args.list:
        for stage in stages:
            print(stage.name + ': ' + json.dumps(stage.command))
        return 0
    if os.name != 'nt' and not args.allow_non_windows:
        parser.error('Run on native Windows; use --allow-non-windows only for harness smoke tests')
    report = {'platform': platform.platform(), 'native_windows': os.name == 'nt',
              'python': sys.version, 'logical_cpus': os.cpu_count(),
              'commit': git('rev-parse', 'HEAD'), 'branch': git('branch', '--show-current'),
              'working_tree_at_start': git('status', '--short'),
              'selection': args.only or 'full', 'status': 'running', 'stages': []}
    folder.mkdir(parents=True)
    scratch = folder / 'scratch'
    scratch.mkdir()
    (folder / 'MANUAL.md').write_text((HERE / 'MANUAL-template.md').read_text(encoding='utf-8'), encoding='utf-8')
    env = dict(os.environ, PYTHONUTF8='1', PYTHONUNBUFFERED='1', NERD_LIB_PATH=str(ROOT / 'mods'),
               TMP=str(scratch), TEMP=str(scratch), TMPDIR=str(scratch))
    for name in ['NERD_PROFILE', 'NERD_PROFILE_LOCKS', 'NERD_MEMORY_PROFILE',
                 'NERD_DEBUG_KEEP_LINK_LLVM', 'NERD_DEBUG_LLVM_SIDECARS']:
        env.pop(name, None)
    report['tool_paths'] = {name: shutil.which(name) for name in
                            ['clang', 'opt', 'llc', 'llvm-lib', 'lld-link', 'llvm-dwarfdump',
                             'lldb', 'just', 'git', 'uv', 'node', 'npm']}
    save(folder, report)
    print('Results: ' + str(folder), flush=True)
    # Version probes are evidence, not prerequisite gates: doctor and builds
    # diagnose SDK/CRT support; optional lldb may be supplied by CodeLLDB.
    with (folder / 'tools.log').open('w', encoding='utf-8') as log:
        for name, path in report['tool_paths'].items():
            log.write(f'\n{name}: {path}\n')
            if path:
                try:
                    result = subprocess.run([path, '--version'], capture_output=True,
                                            text=True, encoding='utf-8', errors='replace', timeout=15)
                    log.write(result.stdout + result.stderr + f'\nexit={result.returncode}\n')
                except (OSError, subprocess.SubprocessError) as error:
                    log.write(str(error) + '\n')
    status = {}
    for stage in stages:
        failed = [name for name in stage.needs if status.get(name) != 'passed']
        if failed:
            item = {'name': stage.name, 'command': stage.command, 'status': 'blocked',
                    'reason': 'Prerequisites failed: ' + ', '.join(failed)}
        else:
            print('Running ' + stage.name + ' (output streams to its log)', flush=True)
            report['stages'].append({'name': stage.name, 'command': stage.command,
                                     'log': stage.name + '.log', 'status': 'running'})
            save(folder, report)
            item = execute(stage, folder, env, args.timeout)
            report['stages'].pop()
        report['stages'].append(item)
        status[stage.name] = item['status']
        save(folder, report)
        print(stage.name + ': ' + item['status'], flush=True)
        if item['status'] in ['interrupted', 'cleanup-failed']:
            break
    report['status'] = 'passed' if len(status) == len(stages) and all(v == 'passed' for v in status.values()) else 'incomplete-or-failed'
    report['finished_utc'] = datetime.now(timezone.utc).isoformat()
    save(folder, report)
    print('Report: ' + str(folder / 'SUMMARY.md'), flush=True)
    return 0 if report['status'] == 'passed' else 1


if __name__ == '__main__':
    raise SystemExit(main())
