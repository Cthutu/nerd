#!/usr/bin/env python3
"""Run Nerd's correctness gates with live status and reviewable failure logs."""
import argparse
from collections import Counter
from datetime import datetime, timezone
import math
import os
import signal
from pathlib import Path
import sys
import uuid

from harness.dashboard import Dashboard, Terminal
from harness.engine import BAD, Runner
from harness.plan import ROOT, make_plan, select


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--profile', choices=['all', 'debug', 'release'], default='all')
    parser.add_argument('--suite', nargs='+', action='extend', default=[], help='Select suite names (repeatable)')
    parser.add_argument('--only', nargs='+', action='extend', default=[], help='Select gate IDs and prerequisites')
    parser.add_argument('--filter', default='', help='Select fixture paths; implies fixture-only unless --suite/--only is given')
    parser.add_argument('--network', action='store_true', help='Include loopback network integrations')
    parser.add_argument('--nexus', action='store_true', help='Include Nexus integrations')
    parser.add_argument('--editors', action='store_true', help='Include installed editor and Neovim checks')
    parser.add_argument('--debugger', action='store_true', help='Include native CodeLLDB smoke/stepping checks')
    parser.add_argument('--plain', action='store_true', help='Use streaming text instead of the terminal dashboard')
    parser.add_argument('--no-pause', action='store_true', help='Exit immediately on failure, including in a terminal')
    parser.add_argument('--skip-build', action='store_true', help='Explicitly reuse existing compiler binaries')
    parser.add_argument('--timeout', type=float, default=1200, help='Maximum seconds per gate (default: 1200)')
    parser.add_argument('--output', type=Path, help='New directory for logs and summary.json')
    parser.add_argument('--list', action='store_true', help='List selected commands without executing them')
    parser.add_argument('--list-suites', action='store_true')
    args = parser.parse_args(argv)
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error('--timeout must be finite and positive')
    wanted = set(args.suite)
    for name in ['network', 'nexus', 'editors', 'debugger']:
        if name in wanted or any(identifier.startswith(name + '-') for identifier in args.only):
            setattr(args, name, True)
    if args.list_suites:
        stages = make_plan(network=True, nexus=True, editors=True, debugger=True)
        for suite in sorted({s.suite for s in stages}):
            optional = ' (opt-in)' if suite in {'network', 'nexus', 'editors', 'debugger'} else ''
            print(suite + optional)
        return 0
    try:
        stages = select(make_plan(profile=args.profile, network=args.network, nexus=args.nexus,
                                  editors=args.editors, debugger=args.debugger, filter_text=args.filter),
                        args.suite, args.only, filtered=bool(args.filter))
    except ValueError as error:
        parser.error(str(error))
    if args.list:
        import json
        for stage in stages:
            print(stage.name + ': ' + json.dumps(stage.command))
        return 0
    folder = args.output or ROOT / '_test-results' / (
        datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex[:8])
    folder = folder.resolve()
    if folder.exists() and not folder.is_dir():
        parser.error(f'Results path is not a directory: {folder}')
    if folder.exists() and any(folder.iterdir()):
        parser.error(f'Results directory is not empty: {folder}')
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    enabled = not args.plain and sys.stdin.isatty() and sys.stdout.isatty() and os.environ.get('TERM') != 'dumb' and not os.environ.get('CI')
    runner = Runner(stages, folder, ROOT, timeout=args.timeout, skip_build=args.skip_build)
    status = 1
    previous_term = signal.getsignal(signal.SIGTERM)

    def interrupt(*_):
        raise KeyboardInterrupt

    signal.signal(signal.SIGTERM, interrupt)
    try:
        with Terminal(enabled) as terminal:
            ui = Dashboard(terminal)
            status = runner.run(ui)
            ui.finish(runner, pause=not args.no_pause)
    except KeyboardInterrupt:
        runner.cancelled = True
        runner.save()
        status = 130
    finally:
        signal.signal(signal.SIGTERM, previous_term)
    counts = Counter(r.status for r in runner.results)
    print(f'\nNERD TESTS {"PASSED" if status == 0 else "CANCELLED" if status == 130 else "FAILED"} '
          f'— {counts["passed"]} passed, {counts["failed"] + counts["timeout"]} failed, '
          f'{counts["skipped"]} skipped, {counts["blocked"]} blocked, {counts["cancelled"]} cancelled')
    for result in runner.results:
        if result.status in BAD:
            print(f'  {result.stage.name}: {result.detail} — {folder / result.log if result.log else "not started"}')
            if result.status not in {'blocked', 'cancelled'}:
                for line in list(result.tail)[-12:]:
                    print('    ' + line)
    print('Results: ' + str(folder / 'summary.json'))
    return status


if __name__ == '__main__':
    raise SystemExit(main())
