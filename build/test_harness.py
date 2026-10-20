#!/usr/bin/env python3
"""Regression checks for grouped fixture dispatch and independent programs."""
from pathlib import Path
from types import SimpleNamespace
import tempfile
import unittest
from unittest.mock import patch

import test as runner


class GroupedFixtures(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)

    def fixture(self, name, text):
        path = self.root / name
        path.write_text(text, encoding="utf-8")
        return path

    def test_language_namespaces_stdin_and_failure_labels(self):
        source = 'answer :: 42\nmain :: fn () => answer\n'
        record = source + '¬\n42\n¬\n\n¬\n\n¬\n\n¬\ninput\n'
        path = self.fixture('functions.t', record + '\n=== case: independent ===\n' + record)
        cases = runner.language_cases(path)
        self.assertEqual([label for label, _ in cases], ['batch', 'independent'])
        self.assertEqual(cases[0][1][0], cases[1][1][0])
        self.assertEqual(cases[1][1][5].strip(), 'input')
        with patch.object(runner, 'test_language_case', side_effect=[[], [runner.Failure(path, 'wrong result')]]) as run:
            failures = runner.test_language(path)
        self.assertEqual(run.call_count, 2)
        self.assertIn('independent: wrong result', failures[0].message)

    def test_formatter_checks_later_pairs_and_rejects_truncation(self):
        path = self.fixture('bindings.f', 'a¬A¬b¬B')
        with patch.object(runner, 'test_format_case', side_effect=[[], [runner.Failure(path, 'wrong format')]]) as run:
            failures = runner.test_format(path)
        self.assertEqual(run.call_args_list[1].args[1:], ('b', 'B'))
        self.assertIn('case 1', failures[0].message)
        path.write_text('a¬A¬b')
        self.assertTrue(runner.test_format(path))

    def test_lsp_checks_later_sessions_and_rejects_truncation(self):
        path = self.fixture('hover.lsp', 'a¬[]¬[]¬b¬[]¬[]')
        with patch.object(runner, 'test_lsp_case', side_effect=[[], [runner.Failure(path, 'wrong hover')]]) as run:
            failures = runner.test_lsp(path)
        self.assertEqual(run.call_args_list[1].args[1], 'b')
        self.assertIn('case 1', failures[0].message)
        path.write_text('a¬[]¬[]¬b')
        self.assertTrue(runner.test_lsp(path))

    def test_semantic_groups_check_every_program_without_linking(self):
        path = self.fixture('ffi.check', '=== case: first ===\nmain :: fn () {}\n=== case: second ===\nmain :: fn () {}\n')
        with patch.object(runner, 'run_cmd', side_effect=[
            SimpleNamespace(returncode=0, stdout='', stderr=''),
            SimpleNamespace(returncode=1, stdout='', stderr='type mismatch')
        ]) as run:
            failures = runner.test_sema(path)
        self.assertEqual(run.call_count, 2)
        self.assertTrue(all(call.args[0][1] == 'check' for call in run.call_args_list))
        self.assertIn('second: semantic check failed', failures[0].message)

    def test_release_metadata_is_a_complete_comment_line(self):
        self.assertEqual(runner.language_flags('-- test-release\nmain :: fn () {}'), ['-r'])
        self.assertEqual(runner.language_flags('main :: fn () { prn("-- test-release") }'), [])


if __name__ == '__main__':
    unittest.main()
