#!/usr/bin/env python3
"""Exercise catalogue coverage, real subprocess failures and terminal lifecycle."""
import contextlib
import io
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
import tempfile
import time
import unittest

TESTS = Path(__file__).resolve().parents[1]
ROOT = TESTS.parent
sys.path.insert(0, str(TESTS))
from harness.dashboard import Dashboard, Terminal, fit, keys
from harness.engine import ProcessTree, Result, Runner, clean
from harness.plan import Stage, make_plan, select


def command(code):
    return (sys.executable, '-c', code)


def probe(mode):
    with tempfile.TemporaryDirectory(prefix='nerd-ui-probe-') as directory:
        if mode == 'cancel':
            code = 'import subprocess,sys,time; p=subprocess.Popen([sys.executable,"-c","import time; time.sleep(60)"], start_new_session=True); print("nested-child="+str(p.pid),flush=True); time.sleep(60)'
        else:
            code = 'import time; time.sleep(.1); print("specific compiler diagnostic",flush=True); raise SystemExit(' + ('7' if mode == 'failure' else '0') + ')'
        stages = [Stage('probe', 'test', 'debug', command(code))]
        runner = Runner(stages, directory, ROOT, timeout=10)
        with Terminal(True) as terminal:
            ui = Dashboard(terminal)
            status = runner.run(ui)
            print('PROBE_EXECUTION_FINISHED', flush=True)
            ui.finish(runner)
        print('PROBE_RETURN=' + str(status), flush=True)
        return status


class CatalogueTests(unittest.TestCase):
    def test_default_has_both_full_profiles_without_optional_integrations(self):
        stages = make_plan()
        self.assertEqual(len(stages), len({s.name for s in stages}))
        for profile in ['debug', 'release']:
            scripts = {s.command[1] for s in stages if s.profile == profile and len(s.command) > 1}
            for name in ['docs', 'frame', 'profile', 'jobs', 'front_threads', 'toolchain', 'install',
                         'format_workflow', 'cgen', 'build_settings', 'windows_stdio',
                         'std_thread_sync', 'raptor', 'std_system', 'render_threads', 'expression_depth']:
                self.assertIn(f'build/test_{name}.py', scripts)
            for backend in ['llvm', 'c']:
                for name in ['raptor', 'std-thread-sync', 'std-system']:
                    self.assertIn(f'{name}-{backend}-{profile}', {s.name for s in stages})
        self.assertFalse({s.suite for s in stages} & {'network', 'nexus', 'editors', 'debugger'})

    def test_selection_dependencies_filter_and_unknowns(self):
        stages = make_plan()
        selected = select(stages, only=['cgen-release'])
        self.assertEqual([s.name for s in selected], ['build-release', 'cgen-release'])
        filtered = select(make_plan(filter_text='maths'), filtered=True)
        self.assertTrue(filtered)
        self.assertTrue(all(s.suite in {'build', 'language', 'stdlib'} for s in filtered))
        with self.assertRaises(ValueError):
            select(stages, suites=['typo'])
        with self.assertRaises(ValueError):
            select(stages, only=['missing'])
        with self.assertRaises(ValueError):
            select(make_plan(filter_text='__no_such_fixture__'), filtered=True)

    def test_optional_network_matrix(self):
        stages = make_plan(network=True, nexus=True)
        for library in ['network', 'nexus']:
            for profile in ['debug', 'release']:
                for backend in ['llvm', 'c']:
                    for target in ['debug', 'release']:
                        result = select(stages, only=[f'{library}-{backend}-{target}-{profile}'])[-1]
                        self.assertEqual('--cgen' in result.command, backend == 'c')
                        self.assertEqual('--release' in result.command, target == 'release')

    def test_just_do_does_not_repeat_release_tests(self):
        recipe = (ROOT / 'Justfile').read_text().split('\ndo:\n')[1].split('\n\n')[0]
        self.assertEqual(recipe.count('just test\n'), 1)
        self.assertNotIn('just test-release', recipe)

    def test_no_match_cli_fails_without_creating_results(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary) / 'results'
            result = subprocess.run([sys.executable, str(TESTS / 'run.py'), '--filter', '__no_such_fixture__',
                                     '--plain', '--output', str(folder)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 2)
            self.assertIn('No tests match', result.stderr)
            self.assertFalse(folder.exists())

    def test_just_preserves_quoted_argument_paths(self):
        with tempfile.TemporaryDirectory(prefix='quoted test path ') as directory:
            output = str(Path(directory) / 'results with spaces')
            result = subprocess.run(['just', 'test', '--only', 'harness', '--list', '--output', output],
                                    cwd=ROOT, capture_output=True, text=True, timeout=15)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('build/test_harness.py', result.stdout)


class ExecutionTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='nerd harness with spaces ')
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.folder = self.root / 'results'
        self.ui = Dashboard(Terminal(False), io.StringIO())

    def test_failed_build_blocks_only_dependents_and_records_independent_success(self):
        stages = [Stage('build', 'build', 'debug', command('print("bad build"); raise SystemExit(7)')),
                  Stage('dependent', 'language', 'debug', command('raise SystemExit(99)'), ('build',)),
                  Stage('independent', 'harness', 'common', command('print("independent passed")'))]
        runner = Runner(stages, self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 1)
        record = json.loads((self.folder / 'summary.json').read_text())
        self.assertEqual([s['status'] for s in record['stages']], ['failed', 'blocked', 'passed'])
        self.assertEqual(record['stages'][0]['exit_code'], 7)
        self.assertIn('bad build', (self.folder / 'build.log').read_text())
        self.assertFalse((self.folder / 'dependent.log').exists())
        self.assertFalse((self.folder / 'summary.tmp').exists())

    def test_missing_command_is_failure(self):
        runner = Runner([Stage('missing', 'test', 'debug', (str(self.root / 'missing'),))], self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 1)
        self.assertEqual(runner.results[0].status, 'failed')
        self.assertTrue((self.folder / 'missing.log').exists())

    def test_timeout_retains_final_output(self):
        runner = Runner([Stage('slow', 'test', 'debug', command('import time; print("before timeout",flush=True); time.sleep(60)'))],
                        self.folder, self.root, timeout=0.2)
        self.assertEqual(runner.run(self.ui), 1)
        self.assertEqual(runner.results[0].status, 'timeout')
        self.assertIn('before timeout', (self.folder / 'slow.log').read_text())

    def test_exit_zero_cannot_override_structured_test_failure(self):
        event = '@@NERD_TEST@@' + json.dumps(dict(status='fail', name='bad.n', detail='precise failure'))
        runner = Runner([Stage('contradiction', 'test', 'debug', command(f'print({event!r})'))], self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 1)
        self.assertIn('precise failure', runner.results[0].failures[0])

    def test_malformed_progress_cannot_pass(self):
        runner = Runner([Stage('protocol', 'test', 'debug', command('print("@@NERD_TEST@@not json")'))], self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 1)
        self.assertIn('Invalid test progress', (self.folder / 'protocol.log').read_text())

    def test_skip_build_is_explicit_and_requires_an_existing_compiler(self):
        stage = Stage('build-debug', 'build', 'debug', command('raise SystemExit(99)'))
        runner = Runner([stage], self.folder, self.root, skip_build=True)
        self.assertEqual(runner.run(self.ui), 1)
        binary = self.root / '_bin' / ('nerd-debug.exe' if os.name == 'nt' else 'nerd-debug')
        binary.parent.mkdir()
        binary.write_bytes(b'existing compiler')
        runner = Runner([stage], self.folder, self.root, skip_build=True)
        self.assertEqual(runner.run(self.ui), 0)
        self.assertIn('Using existing', (self.folder / stage.name).with_suffix('.log').read_text())

    def test_plain_failure_does_not_pause(self):
        runner = Runner([Stage('failed', 'test', 'debug', command('raise SystemExit(3)'))], self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 1)
        self.ui.finish(runner)
        self.assertFalse(self.ui.finished)

    def test_timeout_must_be_finite_and_positive(self):
        for value in [0, -1, float('nan'), float('inf')]:
            with self.assertRaises(ValueError):
                Runner([], self.folder, self.root, timeout=value)

    def test_whole_stage_skip_is_distinct_from_partial_skip(self):
        stages = [Stage('not-native', 'test', 'debug', command('print("[SKIP] Native host required")')),
                  Stage('partial', 'test', 'debug', command('print("[SKIP] One optional check"); print("other contracts passed")'))]
        runner = Runner(stages, self.folder, self.root)
        self.assertEqual(runner.run(self.ui), 0)
        self.assertEqual([r.status for r in runner.results], ['skipped', 'passed'])


class DisplayTests(unittest.TestCase):
    def test_partial_arrow_keys_and_unicode_width(self):
        self.assertEqual(keys('\x1b['), ([], '\x1b['))
        self.assertEqual(keys('\x1b[Aq'), (['up', 'q'], ''))
        self.assertEqual(keys('\x1b', True), (['escape'], ''))
        self.assertEqual(fit('界界', 3), '界 ')
        self.assertNotIn('\x1b', clean('\x1b[31mred\x1b[0m\x1b]0;bad\x07'))

    def test_expected_rejections_are_positive_reports(self):
        state = Result(Stage('raptor', 'raptor', 'debug', ()))
        self.assertIn('[PASS]', state.consume('LLVM debug invalid.n: rejected'))
        self.assertEqual((state.passed, state.failed), (1, 0))

    def test_resize_and_navigation_are_bounded(self):
        with tempfile.TemporaryDirectory() as folder:
            stages = [Stage(str(i), 'test', 'debug', ()) for i in range(20)]
            runner = Runner(stages, folder, ROOT)
            ui = Dashboard(Terminal(False), io.StringIO())
            runner.results[2].status = 'failed'
            runner.results[2].failures = ['specific failure']
            ui.handle_keys(runner, ['f', 'down', 'pageup', 'l', 'pagedown'])
            self.assertEqual(ui.selected, 2)
            for width, height in [(1, 1), (15, 6), (40, 12), (120, 40)]:
                frame = ui.frame(runner, width, height)
                self.assertLessEqual(len(frame), height)
                self.assertTrue(all(len(line) <= width for line, _ in frame))


@unittest.skipIf(os.name == 'nt', 'POSIX pseudo-terminal lifecycle checks')
class TerminalTests(unittest.TestCase):
    def launch(self, mode):
        import fcntl
        import pty
        import struct
        import termios
        master, slave = pty.openpty()
        before = termios.tcgetattr(slave)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', 30, 100, 0, 0))
        tree = ProcessTree()
        process = subprocess.Popen([sys.executable, __file__, '--ui-probe', mode], stdin=slave,
                                   stdout=slave, stderr=slave, start_new_session=True,
                                   env=dict(os.environ, TERM='xterm-256color'))
        self.addCleanup(os.close, master)
        self.addCleanup(os.close, slave)
        self.addCleanup(tree.stop, process)
        return process, master, slave, before

    def collect(self, process, master, until=None, timeout=5):
        import select
        data = b''
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if select.select([master], [], [], 0.05)[0]:
                try:
                    chunk = os.read(master, 65536)
                except OSError:
                    break
                data += chunk
            if until and until in data:
                return data
            if not until and process.poll() is not None:
                return data
        if until:
            self.assertIn(until, data)
        return data

    def test_success_exits_without_input_and_restores_terminal(self):
        import termios
        process, master, slave, before = self.launch('success')
        data = self.collect(process, master)
        self.assertEqual(process.wait(timeout=3), 0)
        self.assertIn(b'\x1b[?1049h', data)
        self.assertIn(b'\x1b[?1049l', data)
        self.assertEqual(termios.tcgetattr(slave), before)

    def test_failure_holds_dashboard_until_acknowledged_and_can_resize(self):
        import fcntl
        import struct
        import termios
        process, master, slave, before = self.launch('failure')
        data = self.collect(process, master, b'PROBE_EXECUTION_FINISHED')
        time.sleep(0.15)
        self.assertIsNone(process.poll())
        self.assertIn(b'specific compiler diagnostic', data)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', 8, 25, 0, 0))
        os.write(master, b'\x1b[B\x1b[5~l')
        time.sleep(0.1)
        self.assertIsNone(process.poll())
        os.write(master, b'\r')
        self.collect(process, master)
        self.assertEqual(process.wait(timeout=3), 1)
        self.assertEqual(termios.tcgetattr(slave), before)

    @unittest.skipUnless(sys.platform.startswith('linux'), 'Linux orphan reaping contract')
    def test_cancel_kills_nested_session_and_restores_terminal(self):
        import termios
        process, master, slave, before = self.launch('cancel')
        data = self.collect(process, master, b'nested-child=')
        deadline = time.monotonic() + 3
        match = re.search(rb'nested-child=(\d+)', data)
        while not match and time.monotonic() < deadline:
            data += self.collect(process, master, timeout=.2)
            match = re.search(rb'nested-child=(\d+)', data)
        self.assertIsNotNone(match)
        pid = int(match[1])
        os.write(master, b'q')
        self.collect(process, master)
        self.assertEqual(process.wait(timeout=3), 130)
        self.assertEqual(termios.tcgetattr(slave), before)
        with self.assertRaises(ProcessLookupError):
            os.kill(pid, 0)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--ui-probe':
        raise SystemExit(probe(sys.argv[2]))
    unittest.main()
