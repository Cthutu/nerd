#!/usr/bin/env python3
"""Prepare and record native desktop checks without changing installed tools."""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', type=Path, help='Validation results directory')
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--observe', action='store_true')
    parser.add_argument('--compiler', choices=['debug', 'release'], nargs='+', default=['debug', 'release'])
    parser.add_argument('--example', choices=['pixels', 'dungeon', 'triangle'], nargs='+', default=['pixels', 'dungeon', 'triangle'])
    args = parser.parse_args()
    if os.name != 'nt':
        parser.error('Native Windows is required')
    if not args.prepare and not args.observe:
        parser.error('Select --prepare and/or --observe')
    folder = args.folder.resolve()
    folder.mkdir(parents=True, exist_ok=True)
    scratch = folder / 'scratch' / 'manual'
    scratch.mkdir(parents=True, exist_ok=True)
    manifest = folder / 'desktop.json'
    report = json.loads(manifest.read_text(encoding='utf-8')) if manifest.exists() else {'cases': {}}
    env = dict(os.environ, NERD_LIB_PATH=str(ROOT / 'mods'))

    def save():
        manifest.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')

    for compiler in args.compiler:
        nerd = ROOT / '_bin' / ('nerd-debug.exe' if compiler == 'debug' else 'nerd.exe')
        for example in args.example:
            source = ROOT / 'examples' / example / (example + '.n')
            for target in ['debug', 'release']:
                for jobs in [1, 4]:
                    for backend in ['llvm', 'c']:
                        key = f'{compiler}-{example}-{target}-j{jobs}-{backend}'
                        output = scratch / (key + '.exe')
                        if args.prepare:
                            case = {'compiler': compiler, 'example': example, 'target': target,
                                    'jobs': jobs, 'backend': backend, 'status': 'NOT RUN',
                                    'commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
                                    'compiler_sha256': hashlib.sha256(nerd.read_bytes()).hexdigest(),
                                    'commands': [], 'log': key + '.log'}
                            report['cases'][key] = case
                            command = [str(nerd), 'build', '--jobs', str(jobs), *(['-r'] if target == 'release' else []),
                                       *(['--cgen', '--copts'] if backend == 'c' else []), str(source), '-o', str(output)]
                            with (folder / case['log']).open('wb') as log:
                                case['commands'].append(command)
                                built = subprocess.run(command, env=env, cwd=ROOT, capture_output=True, timeout=180)
                                log.write(built.stdout + built.stderr)
                                if built.returncode == 0 and backend == 'c':
                                    # copts emits shell-quoted arguments; parse them as an
                                    # argument list instead of PowerShell word splitting.
                                    command = ['clang', str(output.with_suffix('.c')),
                                               *shlex.split(built.stdout.decode('utf-8')), '-o', str(output)]
                                    case['commands'].append(command)
                                    built = subprocess.run(command, env=env, cwd=ROOT, stdout=log, stderr=log, timeout=180)
                                case['build_exit'] = built.returncode
                                if built.returncode:
                                    case['status'] = 'BUILD FAILED'
                            save()
                            print(key, 'prepared' if not built.returncode else 'BUILD FAILED', flush=True)
                        if args.observe:
                            case = report['cases'].get(key)
                            if not case or case.get('build_exit') != 0 or not output.exists():
                                print(key, 'BLOCKED: prepare successfully first', flush=True)
                                continue
                            print('\n' + key, flush=True)
                            print('Check visible drawing, input/resize response, then Q/Escape or normal close.\n'
                                  'For Dungeon, confirm it draws BEFORE pressing any key, move, then Q.\n'
                                  'Compare generated C with LLVM output.', flush=True)
                            input('Press Enter to launch: ')
                            process = subprocess.Popen([str(output)], cwd=source.parent, env=env,
                                                       creationflags=subprocess.CREATE_NEW_CONSOLE)
                            code = process.wait()
                            answer = input(f'Exit={code}. Enter PASS, FAIL or BLOCKED plus observations: ').strip()
                            status, _, notes = answer.partition(' ')
                            if status.upper() not in ['PASS', 'FAIL', 'BLOCKED']:
                                status, notes = 'BLOCKED', 'Unrecognised observation: ' + answer
                            case.update(status=status.upper(), observation=notes, exit_code=code,
                                        observed_utc=datetime.now(timezone.utc).isoformat())
                            save()
    print('Desktop evidence: ' + str(manifest), flush=True)


if __name__ == '__main__':
    main()
