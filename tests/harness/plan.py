"""One catalogue for normal gates, focused runs and native validation."""
from dataclasses import dataclass
from pathlib import Path
import os
import sys

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / 'build'
if str(BUILD) not in sys.path:
    sys.path.insert(0, str(BUILD))
import test as fixtures


@dataclass(frozen=True)
class Stage:
    name: str
    suite: str
    profile: str
    command: tuple[str, ...]
    needs: tuple[str, ...] = ()
    fixture: bool = False


KINDS = tuple(fixtures.SUITE_LABELS)


def make_plan(root=ROOT, profile='all', *, network=False, nexus=False,
              editors=False, debugger=False, filter_text='', split_fixtures=True):
    python = sys.executable
    stages = []

    def add(name, suite, mode, command, needs=(), fixture=False):
        stages.append(Stage(name, suite, mode, tuple(map(str, command)), tuple(needs), fixture))

    common = [
        ('dashboard-harness', 'harness', 'tests/harness/check_runner.py'),
        ('harness', 'harness', 'build/test_harness.py'),
        ('dist', 'harness', 'build/test_dist.py'),
        ('clean', 'harness', 'build/test_clean.py'),
        ('memory', 'memory', 'build/test_memory.py'),
        ('runtime-threads', 'threads', 'build/test_runtime_threads.py'),
        ('threads', 'threads', 'build/test_threads.py'),
        ('benchmark-accounting', 'harness', 'build/test_benchmark_usage.py'),
        ('semantic-headroom', 'harness', 'build/test_semantic_headroom.py'),
        ('windows-harness', 'harness', 'tests/windows/test_runner.py'),
    ]
    for name, suite, script in common:
        add(name, suite, 'common', [python, script])
    modes = ('debug', 'release') if profile == 'all' else (profile,)
    for mode in modes:
        build = 'build-' + mode
        flags = ['-r'] if mode == 'release' else []
        add(build, 'build', mode, [python, 'build/build.py', *flags, 'nerd', '--skip-mod-sync'])
        compiler = root / '_bin' / (('nerd-debug' if mode == 'debug' else 'nerd') + ('.exe' if os.name == 'nt' else ''))
        needs = (build,)
        fixture_args = ['--filter', filter_text] if filter_text else []
        if split_fixtures:
            available = {kind for kind, path in fixtures.collect(filter_text)
                         if filter_text in fixtures.rel(path)}
            for kind in KINDS:
                if kind in available:
                    add(f'{kind}-{mode}', kind, mode,
                        [python, 'build/test.py', '--nerd', compiler, '--kind', kind, '--events', *fixture_args], needs, fixture=True)
        else:
            add('fixtures' if mode == 'debug' else 'fixtures-release', 'fixtures', mode,
                [python, 'build/test.py', '--nerd', compiler, *fixture_args], needs, fixture=True)
        add('doctor-' + mode, 'toolchain', mode, [compiler, 'doctor'], needs)
        add('docs-' + mode, 'docs', mode, [python, 'build/test_docs.py', '--nerd', compiler], needs)
        add('frame-' + mode, 'frame', mode, [python, 'build/test_frame.py', '--nerd', compiler], needs)
        add('stdlib-capabilities-' + mode, 'capabilities', mode,
            [python, 'tests/stdlib-capabilities/check_capabilities.py', '--compiler', compiler], needs)
        for name, suite in [
            ('profile', 'profiling'), ('render_threads', 'threads'),
            ('expression_depth', 'compiler'), ('jobs', 'scheduling'),
            ('front_threads', 'threads'), ('toolchain', 'toolchain'),
            ('install', 'installation'), ('format_workflow', 'format'),
            ('cgen', 'cgen'), ('build_settings', 'configuration'),
            ('windows_stdio', 'stdio'),
        ]:
            add(name.replace('_', '-') + '-' + mode, suite, mode,
                [python, f'build/test_{name}.py', '--nerd', compiler], needs)
        for backend, flags in [('llvm', []), ('c', ['--cgen'])]:
            for name, suite in [('std_thread_sync', 'threads'), ('raptor', 'raptor'), ('std_system', 'system')]:
                add(f'{name.replace("_", "-")}-{backend}-{mode}', suite, mode,
                    [python, f'build/test_{name}.py', '--nerd', compiler, *flags], needs)
            for name, enabled in [('network', network), ('nexus', nexus)]:
                if enabled:
                    for target, target_flags in [('debug', []), ('release', ['--release'])]:
                        add(f'{name}-{backend}-{target}-{mode}', name, mode,
                            [python, f'build/test_{name}.py', '--compiler', compiler, *flags, *target_flags], needs)
        if editors:
            add('editor-' + mode, 'editors', mode,
                [python, 'build/check_editor_integrations.py', '--nerd', compiler], needs)
        if debugger:
            for script, name in [('smoke', 'debugger-smoke'), ('stepping', 'debugger')]:
                add(name + '-' + mode, 'debugger', mode,
                    [python, f'build/check_debugger_{script}.py', '--nerd', compiler], needs)
    if editors or debugger:
        add('adapter-transforms', 'editors' if editors else 'debugger', 'common',
            [python, 'build/check_debugger_adapter_transforms.py'])
    if editors:
        add('nvim-indent', 'editors', 'common', [python, 'build/check_nvim_indent.py'])
    return stages


def select(stages, suites=(), only=(), *, filtered=False):
    known = {s.name: s for s in stages}
    if len(known) != len(stages):
        raise ValueError('Duplicate stage IDs in the test catalogue')
    unknown_suites = set(suites) - {s.suite for s in stages}
    unknown_ids = set(only) - known.keys()
    if unknown_suites or unknown_ids:
        raise ValueError('Unknown selection: ' + ', '.join(sorted(unknown_suites | unknown_ids)))
    if suites or only:
        chosen = {s.name for s in stages if s.suite in suites} | set(only)
    elif filtered:
        chosen = {s.name for s in stages if s.fixture}
    else:
        chosen = set(known)
    if not chosen:
        raise ValueError('No tests match the selection')
    pending = list(chosen)
    while pending:
        stage = known[pending.pop()]
        for dependency in stage.needs:
            if dependency not in known:
                raise ValueError(f'{stage.name}: unknown prerequisite {dependency}')
            if dependency not in chosen:
                chosen.add(dependency)
                pending.append(dependency)
    selected = [s for s in stages if s.name in chosen]
    seen = set()
    for stage in selected:
        if not set(stage.needs) <= seen:
            raise ValueError(f'{stage.name}: prerequisites must precede their dependents')
        seen.add(stage.name)
    return selected
