"""Serial gate execution, bounded subprocess trees and persistent results."""
from collections import deque
from dataclasses import dataclass, field
from datetime import datetime, timezone
import ctypes
import json
import math
import os
from pathlib import Path
import queue
import re
import signal
import subprocess
import sys
import threading
import time

from .plan import Stage

PREFIX = '@@NERD_TEST@@'
ANSI = re.compile(r'\x1b\][^\x07]*(?:\x07|\x1b\\)|\x1b\[[0-?]*[ -/]*[@-~]')
BAD = {'failed', 'timeout', 'blocked', 'cancelled'}


def clean(text):
    return ''.join(c for c in ANSI.sub('', text).replace('\r', '')
                   if c in '\n\t' or ord(c) >= 32 and ord(c) != 127)


@dataclass
class Result:
    stage: Stage
    status: str = 'pending'
    seconds: float = 0.0
    started: float = 0.0
    exit_code: int | None = None
    detail: str = ''
    current: str = ''
    total: int = 0
    passed: int = 0
    failed: int = 0
    skipped: int = 0
    structured_failed: bool = False
    protocol_error: bool = False
    only_skips: bool = True
    failures: list = field(default_factory=list)
    log: str = ''
    tail: deque = field(default_factory=lambda: deque(maxlen=2000))

    def record(self):
        return dict(name=self.stage.name, suite=self.stage.suite, profile=self.stage.profile,
                    command=list(self.stage.command), needs=list(self.stage.needs),
                    status=self.status, seconds=round(self.seconds, 3), exit_code=self.exit_code,
                    detail=self.detail, current=self.current, total=self.total,
                    passed=self.passed, failed=self.failed, skipped=self.skipped,
                    failures=self.failures, log=self.log)

    def consume(self, raw):
        line = clean(raw.rstrip('\n'))
        if line.startswith(PREFIX):
            try:
                event = json.loads(line[len(PREFIX):])
                status = event['status']
                if status == 'suite':
                    if type(event['total']) is not int or event['total'] < 0:
                        raise ValueError('invalid total')
                    self.total = event['total']
                    return ''
                if status not in {'start', 'pass', 'fail', 'skip'}:
                    raise ValueError('invalid status')
                self.current = str(event['name'])
                if status == 'start':
                    return ''
                attr = {'pass': 'passed', 'fail': 'failed', 'skip': 'skipped'}[status]
                setattr(self, attr, getattr(self, attr) + 1)
                self.only_skips &= status == 'skip'
                self.structured_failed |= status == 'fail'
                line = f'[{status.upper()}] {self.current}'
                if event.get('detail'):
                    line += '\n' + clean(str(event['detail']))
                if status == 'fail':
                    self.failures.append(line)
            except (KeyError, TypeError, ValueError) as error:
                self.protocol_error = True
                self.only_skips = False
                line = 'Invalid test progress event: ' + str(error) + '\n' + line
                self.failures.append(line)
        else:
            match = re.match(r'^\[(PASS|FAIL|SKIP)\]\s*(.*)', line)
            if match:
                status, self.current = match.groups()
                attr = {'PASS': 'passed', 'FAIL': 'failed', 'SKIP': 'skipped'}[status]
                setattr(self, attr, getattr(self, attr) + 1)
                self.only_skips &= status == 'SKIP'
            elif line.startswith(('SKIP ',)) or 'regression skipped:' in line or 'tests skipped on ' in line:
                self.skipped += 1
                self.current = line
                line = '[SKIP] ' + line
            elif line.endswith(': rejected'):
                self.only_skips = False
                self.passed += 1
                self.current = line[:-len(': rejected')]
                line = '[PASS] ' + self.current + ' (expected assertion rejected)'
            elif line.endswith(': passed'):
                self.only_skips = False
                self.passed += 1
                self.current = line[:-len(': passed')]
            elif line.strip():
                self.only_skips = False
                self.current = line.strip()
        self.tail.extend(line.splitlines())
        return line


def linux_children(pid):
    try:
        return {int(p) for p in Path(f'/proc/{pid}/task/{pid}/children').read_text().split()}
    except (OSError, ValueError):
        return set()


class ProcessTree:
    """Keep orphaned Linux descendants, including new sessions, owned by us."""
    def __init__(self):
        self.subreaper = False
        if sys.platform.startswith('linux'):
            try:
                self.subreaper = ctypes.CDLL(None, use_errno=True).prctl(36, 1, 0, 0, 0) == 0
            except (AttributeError, OSError):
                pass
        self.baseline = linux_children(os.getpid())

    def stop(self, process):
        if os.name == 'nt':
            if process.poll() is None:
                subprocess.run(['taskkill', '/PID', str(process.pid), '/T', '/F'],
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                               timeout=15, check=False)
        else:
            pids = {process.pid}
            pending = [process.pid]
            while pending:
                for child in linux_children(pending.pop()) - pids:
                    pids.add(child)
                    pending.append(child)
            if self.subreaper:
                pids |= linux_children(os.getpid()) - self.baseline
            groups = {process.pid}
            for pid in pids:
                try:
                    groups.add(os.getpgid(pid))
                except ProcessLookupError:
                    pass
            groups.discard(os.getpgrp())
            for sig in (signal.SIGTERM, signal.SIGKILL):
                for group in groups:
                    try:
                        os.killpg(group, sig)
                    except ProcessLookupError:
                        pass
                if sig == signal.SIGTERM:
                    time.sleep(0.05)
        if process.poll() is None:
            process.kill()
        process.wait(timeout=10)
        if self.subreaper:
            # Terminating a parent can reparent another generation to us.
            for _ in range(4):
                children = linux_children(os.getpid()) - self.baseline
                if not children:
                    break
                for pid in children:
                    try:
                        os.kill(pid, signal.SIGKILL)
                        deadline = time.monotonic() + 2
                        while time.monotonic() < deadline:
                            reaped, _ = os.waitpid(pid, os.WNOHANG)
                            if reaped:
                                break
                            time.sleep(0.01)
                    except (ProcessLookupError, ChildProcessError):
                        pass


class Runner:
    def __init__(self, stages, folder, root, env=None, timeout=1200, skip_build=False):
        if not math.isfinite(timeout) or timeout <= 0:
            raise ValueError('Gate timeout must be finite and positive')
        self.results = [Result(stage, log=stage.name + '.log') for stage in stages]
        self.folder = Path(folder)
        self.root = Path(root)
        self.env = dict(os.environ if env is None else env)
        self.env.update(PYTHONUTF8='1', PYTHONUNBUFFERED='1', NERD_LIB_PATH=str(self.root / 'mods'))
        for key in ['NERD_PROFILE', 'NERD_PROFILE_LOCKS', 'NERD_MEMORY_PROFILE',
                    'NERD_DEBUG_KEEP_LINK_LLVM', 'NERD_DEBUG_LLVM_SIDECARS']:
            self.env.pop(key, None)
        self.timeout = timeout
        self.skip_build = skip_build
        self.cancelled = False
        self.started = time.monotonic()
        self.metadata = dict(root=str(self.root), started_utc=datetime.now(timezone.utc).isoformat())
        for name, args in [('commit', ['rev-parse', 'HEAD']), ('branch', ['branch', '--show-current']),
                           ('dirty', ['status', '--porcelain'])]:
            try:
                value = subprocess.check_output(['git', *args], cwd=self.root, text=True,
                                                stderr=subprocess.DEVNULL, timeout=5).strip()
                self.metadata[name] = bool(value) if name == 'dirty' else value
            except (OSError, subprocess.SubprocessError):
                self.metadata[name] = None if name == 'dirty' else 'unavailable'
        self.folder.mkdir(parents=True, exist_ok=True)
        self.save()

    def save(self):
        record = dict(self.metadata, schema=1, status='cancelled' if self.cancelled else
                      'failed' if any(r.status in BAD for r in self.results) else
                      'running' if any(r.status in {'running', 'pending'} for r in self.results) else 'passed',
                      seconds=round(time.monotonic() - self.started, 3),
                      stages=[r.record() for r in self.results])
        temporary = self.folder / 'summary.tmp'
        temporary.write_text(json.dumps(record, indent=2) + '\n', encoding='utf-8')
        temporary.replace(self.folder / 'summary.json')

    def execute(self, result, ui):
        stage = result.stage
        result.status = 'running'
        result.started = time.monotonic()
        self.save()
        ui.changed(self, result)
        if self.skip_build and stage.suite == 'build':
            binary = self.root / '_bin' / (('nerd-debug' if stage.profile == 'debug' else 'nerd') + ('.exe' if os.name == 'nt' else ''))
            result.status = 'passed' if binary.is_file() else 'failed'
            result.detail = 'Using existing compiler' if binary.is_file() else f'Compiler not found: {binary}'
            (self.folder / result.log).write_text(result.detail + '\n', encoding='utf-8')
            return
        messages = queue.Queue()
        process = None
        tree = ProcessTree()
        with (self.folder / result.log).open('w', encoding='utf-8') as log:
            try:
                process = subprocess.Popen(stage.command, cwd=self.root, env=self.env,
                                           stdin=subprocess.DEVNULL, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT, text=True, encoding='utf-8',
                                           errors='replace', start_new_session=os.name != 'nt',
                                           creationflags=subprocess.CREATE_NEW_PROCESS_GROUP if os.name == 'nt' else 0)

                def read():
                    try:
                        for line in process.stdout:
                            messages.put(line)
                    finally:
                        messages.put(None)

                reader = threading.Thread(target=read, daemon=True)
                reader.start()
                eof = False
                exited_at = None
                while not eof or process.poll() is None:
                    ui.tick(self, result)
                    if ui.cancelled:
                        self.cancelled = True
                        result.status = 'cancelled'
                        result.detail = 'Cancelled by user'
                        break
                    result.seconds = time.monotonic() - result.started
                    if result.seconds > self.timeout:
                        result.status = 'timeout'
                        result.detail = f'Exceeded {self.timeout:g}s deadline'
                        break
                    if process.poll() is not None:
                        exited_at = exited_at or time.monotonic()
                        if not eof and time.monotonic() - exited_at > 1:
                            tree.stop(process)
                    try:
                        raw = messages.get(timeout=0.05)
                    except queue.Empty:
                        continue
                    if raw is None:
                        eof = True
                    else:
                        line = result.consume(raw)
                        if line:
                            log.write(line + '\n')
                            log.flush()
                            ui.output(result, line)
                if result.status == 'running':
                    result.exit_code = process.wait(timeout=10)
                    result.status = 'passed' if result.exit_code == 0 and not result.structured_failed and not result.protocol_error else 'failed'
                    if result.status == 'passed' and result.only_skips and result.skipped:
                        result.status = 'skipped'
                    if result.status == 'failed':
                        result.detail = f'Exit {result.exit_code}' if result.exit_code else 'Test progress reported failure'
            except KeyboardInterrupt:
                self.cancelled = True
                result.status = 'cancelled'
                result.detail = 'Interrupted'
            except (OSError, subprocess.SubprocessError) as error:
                result.status = 'failed'
                result.detail = clean(str(error))
            finally:
                if process is not None:
                    tree.stop(process)
                    if result.exit_code is None:
                        result.exit_code = process.returncode
                    reader.join(timeout=5)
                    process.stdout.close()
                    # Keep final diagnostics produced during termination.
                    while not messages.empty():
                        raw = messages.get_nowait()
                        if raw is not None:
                            line = result.consume(raw)
                            if line:
                                log.write(line + '\n')
                if result.detail:
                    log.write(result.detail + '\n')
                    result.tail.append(result.detail)

    def run(self, ui):
        known = {}
        try:
            for result in self.results:
                if self.cancelled:
                    result.status = 'cancelled'
                    result.detail = 'Run interrupted before this gate started'
                    result.log = ''
                else:
                    blocked = [n for n in result.stage.needs if known[n].status != 'passed']
                    if blocked:
                        result.status = 'blocked'
                        result.detail = 'Prerequisites failed: ' + ', '.join(blocked)
                        result.log = ''
                    else:
                        self.execute(result, ui)
                if result.started:
                    result.seconds = time.monotonic() - result.started
                known[result.stage.name] = result
                self.save()
                ui.changed(self, result)
        except KeyboardInterrupt:
            self.cancelled = True
            for result in self.results:
                if result.status in {'running', 'pending'}:
                    result.status = 'cancelled'
            self.save()
        return 130 if self.cancelled else 1 if any(r.status in BAD for r in self.results) else 0
