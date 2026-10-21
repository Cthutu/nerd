#!/usr/bin/env python3
"""Harness-only tests; no compiler or Windows validation claims."""
import json
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

import run


class RunnerTests(unittest.TestCase):
    def test_library_matrix_uses_requested_compiler_backend_and_target(self):
        stages = run.plan(Path('results'))
        for mode in ['debug', 'release']:
            suffix = '.exe' if os.name == 'nt' else ''
            compiler = str(run.ROOT / '_bin' / (('nerd-debug' if mode == 'debug' else 'nerd') + suffix))
            for backend in ['llvm', 'c']:
                for target in ['debug', 'release']:
                    for library in ['network', 'nexus']:
                        name = f'{library}-{backend}-{target}-{mode}'
                        selected = run.select(stages, [name])
                        self.assertEqual([s.name for s in selected], ['build-' + mode, name])
                        command = selected[-1].command
                        self.assertEqual(command[1:4], [f'build/test_{library}.py', '--compiler', compiler])
                        self.assertEqual('--cgen' in command, backend == 'c')
                        self.assertEqual('--release' in command, target == 'release')

    def test_plan_covers_just_test_scripts(self):
        # Keep newly added common gates from silently missing native validation.
        import re
        recipes = (run.ROOT / 'Justfile').read_text().split('\ntest *args:', 1)[1].split('\ntest-build:', 1)[0]
        scripts = set(re.findall(r'python3 ([\w/]+\.py)', recipes))
        planned = {s.command[1] for s in run.plan(Path('results')) if len(s.command) > 1}
        self.assertFalse(scripts - planned, f'Missing scripts: {scripts - planned}')

    def test_dependencies_and_unknown_stage(self):
        stages = run.plan(Path('results with spaces').resolve())
        selected = run.select(stages, ['front-threads-release'])
        self.assertEqual([s.name for s in selected], ['build-release', 'front-threads-release'])
        self.assertEqual(run.select(stages, ['benchmark']), stages)
        with self.assertRaises(ValueError):
            run.select(stages, ['typo'])
        self.assertEqual(len({s.name for s in stages}), len(stages))

    def test_record_success_failure_missing_and_timeout(self):
        with tempfile.TemporaryDirectory(prefix='validation with spaces ') as directory:
            folder = Path(directory)
            env = dict(os.environ, PYTHONUTF8='1')
            for name, code, expected in [
                ('ok', 'print("unicode: \\u00ac")', 'passed'),
                ('bad', 'print("diagnostic", flush=True); raise SystemExit(7)', 'failed'),
                ('slow', 'import time; print("started", flush=True); time.sleep(30)', 'timeout'),
            ]:
                result = run.execute(run.Stage(name, [sys.executable, '-c', code]), folder,
                                     env, 0.2 if name == 'slow' else 10, cwd=folder)
                self.assertEqual(result['status'], expected)
                self.assertTrue((folder / result['log']).is_file())
                if name == 'bad':
                    self.assertEqual(result['exit_code'], 7)
                    self.assertIn('diagnostic', (folder / result['log']).read_text())
            missing = run.execute(run.Stage('missing', [str(folder / 'does-not-exist.exe')]),
                                  folder, env, 1, cwd=folder)
            self.assertEqual(missing['status'], 'failed')
            self.assertIn('error', missing)

    def test_failed_build_blocks_dependents_but_keeps_independent_results(self):
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'MANUAL-template.md').write_text('NOT RUN')
            stages = [run.Stage('build-debug', [sys.executable, '-c', 'raise SystemExit(7)']),
                      run.Stage('dependent', [sys.executable, '-c', 'raise SystemExit(99)'], ('build-debug',)),
                      run.Stage('independent', [sys.executable, '-c', 'print("ok")'])]
            with patch.object(run, 'HERE', folder), patch.object(run, 'plan', return_value=stages), \
                 patch.object(run, 'git', return_value='test'), patch.object(run.shutil, 'which', return_value=None), \
                 patch.object(sys, 'argv', ['run.py', '--allow-non-windows']):
                self.assertEqual(run.main(), 1)
            summaries = list((folder / 'results').glob('*/summary.json'))
            self.assertEqual(len(summaries), 1)
            report = json.loads(summaries[0].read_text())
            self.assertEqual([s['status'] for s in report['stages']], ['failed', 'blocked', 'passed'])
            self.assertFalse((summaries[0].parent / 'dependent.log').exists())

    def test_report_persists_incomplete_status(self):
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            report = {'platform': 'test host', 'commit': 'abc', 'branch': 'experiment',
                      'selection': ['jobs-debug'], 'status': 'incomplete-or-failed',
                      'stages': [{'name': 'jobs-debug', 'status': 'blocked',
                                  'reason': 'Prerequisites failed: build-debug'}]}
            run.save(folder, report)
            self.assertEqual(json.loads((folder / 'summary.json').read_text()), report)
            self.assertIn('blocked', (folder / 'SUMMARY.md').read_text())
            self.assertFalse((folder / 'summary.tmp').exists())


if __name__ == '__main__':
    unittest.main()
