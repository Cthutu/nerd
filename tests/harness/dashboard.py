"""Dependency-free, full-screen terminal status and failure/log navigation."""
import ctypes
import os
from pathlib import Path
import select
import shutil
import sys
import time
import unicodedata

from .engine import BAD, clean

COLOURS = {'passed': '32', 'failed': '31', 'timeout': '31', 'blocked': '33',
           'cancelled': '33', 'running': '36', 'pending': '90', 'skipped': '33'}
ARROWS = {'\x1b[A': 'up', '\x1b[B': 'down', '\x1b[5~': 'pageup', '\x1b[6~': 'pagedown',
          '\x1b[H': 'home', '\x1b[F': 'end'}


def fit(text, width):
    result = ''
    used = 0
    for char in clean(text).replace('\t', '    ').replace('\n', ' '):
        size = 0 if unicodedata.combining(char) else 2 if unicodedata.east_asian_width(char) in 'WF' else 1
        if used + size > width:
            break
        result += char
        used += size
    return result + ' ' * max(0, width - used)


def keys(buffer, flush=False):
    decoded = []
    while buffer:
        match = next((k for k in ARROWS if buffer.startswith(k)), None)
        if match:
            decoded.append(ARROWS[match])
            buffer = buffer[len(match):]
        elif any(k.startswith(buffer) for k in ARROWS) and not flush:
            break
        else:
            char, buffer = buffer[0], buffer[1:]
            decoded.append({'\r': 'enter', '\n': 'enter', '\x1b': 'escape', '\x03': 'cancel'}.get(char, char))
    return decoded, buffer


class Terminal:
    def __init__(self, enabled):
        self.enabled = enabled
        self.saved = None
        self.buffer = ''
        self.last_input = 0.0

    def __enter__(self):
        if not self.enabled:
            return self
        if os.name == 'nt':
            from ctypes import wintypes
            kernel = ctypes.windll.kernel32
            kernel.GetStdHandle.argtypes = [wintypes.DWORD]
            kernel.GetStdHandle.restype = wintypes.HANDLE
            kernel.GetConsoleMode.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
            kernel.SetConsoleMode.argtypes = [wintypes.HANDLE, wintypes.DWORD]
            handle = kernel.GetStdHandle(-11)
            mode = ctypes.c_ulong()
            if kernel.GetConsoleMode(handle, ctypes.byref(mode)):
                self.saved = (handle, mode.value)
                kernel.SetConsoleMode(handle, mode.value | 4)
        else:
            import termios
            import tty
            self.saved = termios.tcgetattr(sys.stdin.fileno())
            tty.setcbreak(sys.stdin.fileno())
        sys.stdout.write('\x1b[?1049h\x1b[?25l')
        sys.stdout.flush()
        return self

    def __exit__(self, *_):
        if not self.enabled:
            return
        try:
            if os.name == 'nt':
                if self.saved:
                    ctypes.windll.kernel32.SetConsoleMode(*self.saved)
            else:
                import termios
                termios.tcsetattr(sys.stdin.fileno(), termios.TCSADRAIN, self.saved)
        finally:
            sys.stdout.write('\x1b[0m\x1b[?25h\x1b[?1049l')
            sys.stdout.flush()

    def poll(self):
        if not self.enabled:
            return []
        incoming = ''
        if os.name == 'nt':
            import msvcrt
            while msvcrt.kbhit():
                char = msvcrt.getwch()
                if char in ('\x00', '\xe0'):
                    code = msvcrt.getwch()
                    incoming += {'H': '\x1b[A', 'P': '\x1b[B', 'I': '\x1b[5~', 'Q': '\x1b[6~'}.get(code, '')
                else:
                    incoming += char
        elif select.select([sys.stdin], [], [], 0)[0]:
            incoming = os.read(sys.stdin.fileno(), 1024).decode('utf-8', errors='replace')
        if incoming:
            self.last_input = time.monotonic()
            self.buffer += incoming
        result, self.buffer = keys(self.buffer, time.monotonic() - self.last_input > 0.1)
        return result


class Dashboard:
    def __init__(self, terminal, plain_stream=None):
        self.terminal = terminal
        self.stream = plain_stream or sys.stdout
        self.cancelled = False
        self.selected = 0
        self.follow = True
        self.failures_only = False
        self.offset = 0
        self.last_draw = 0.0
        self.finished = False
        self.full_log = False

    def output(self, result, line):
        if not self.terminal.enabled:
            print(line, file=self.stream, flush=True)

    def changed(self, runner, result):
        if self.follow:
            self.selected = runner.results.index(result)
            self.offset = 0
        if not self.terminal.enabled:
            if result.status == 'running':
                print(f'\n[RUN] {result.stage.name}', file=self.stream, flush=True)
            else:
                print(f'[{result.status.upper()}] {result.stage.name} ({result.seconds:.1f}s) {result.detail}',
                      file=self.stream, flush=True)
        else:
            self.draw(runner, force=True)

    def candidates(self, runner):
        return [i for i, r in enumerate(runner.results)
                if not self.failures_only or r.status in BAD or r.failed]

    def handle_keys(self, runner, incoming):
        for key in incoming:
            if key in {'q', 'escape', 'cancel'}:
                self.cancelled = True
            elif key in {'j', 'down', 'k', 'up'}:
                available = self.candidates(runner)
                if available:
                    current = available.index(self.selected) if self.selected in available else 0
                    current += 1 if key in {'j', 'down'} else -1
                    self.selected = available[max(0, min(current, len(available) - 1))]
                    self.follow = False
                    self.offset = 0
            elif key == 'f':
                self.failures_only = not self.failures_only
                available = self.candidates(runner)
                if available and self.selected not in available:
                    self.selected = available[0]
                self.follow = False
                self.offset = 0
            elif key == 'g':
                self.follow = True
                self.failures_only = False
                active = [i for i, r in enumerate(runner.results) if r.status == 'running']
                if active:
                    self.selected = active[0]
                self.offset = 0
            elif key == 'pageup':
                self.offset += 10
            elif key == 'pagedown':
                self.offset = max(0, self.offset - 10)
            elif key == 'end':
                self.offset = 0
            elif key == 'l':
                self.full_log = not self.full_log
                self.offset = 0
    def tick(self, runner, result=None):
        self.handle_keys(runner, self.terminal.poll())
        if self.terminal.enabled:
            self.draw(runner)

    def frame(self, runner, width, height):
        width, height = max(1, width), max(1, height)
        elapsed = int(time.monotonic() - runner.started)
        done = sum(r.status not in {'running', 'pending'} for r in runner.results)
        passed = sum(r.status == 'passed' for r in runner.results)
        failed = sum(r.status in BAD for r in runner.results)
        total = len(runner.results)
        title = 'NERD  /  TEST CONTROL'
        if self.finished:
            title += '  /  FAILED' if failed else '  /  PASSED'
        lines = [(title + f'     {elapsed // 60:02}:{elapsed % 60:02}', '35')]
        profiles = []
        for profile in ('common', 'debug', 'release'):
            group = [r for r in runner.results if r.stage.profile == profile]
            if group:
                completed = sum(r.status not in {'running', 'pending'} for r in group)
                profiles.append(f'{profile.upper()} {completed}/{len(group)}')
        lines.append(('   │   '.join(profiles), '90'))
        bar_width = max(1, min(40, width - 25))
        fill = int(bar_width * done / max(1, total))
        lines.append(('[' + '━' * fill + '·' * (bar_width - fill) + f'] {done}/{total} gates   {passed} passed  {failed} failed/blocked', '36'))
        if height < 12:
            if runner.results:
                selected = runner.results[min(self.selected, total - 1)]
                lines.append((selected.stage.name + ': ' + selected.status, COLOURS[selected.status]))
                lines.append((selected.current or selected.detail, '0'))
            lines.append(('Enlarge terminal for logs | q: cancel/exit', '90'))
            return [(fit(text, width), colour) for text, colour in lines[:height]]
        lines.append(('─ SUITES  ↑/↓ or j/k select   f failures   g follow ' + '─' * width, '90'))
        available = self.candidates(runner)
        row_count = min(max(3, height // 3), max(1, len(available)))
        location = available.index(self.selected) if self.selected in available else 0
        start = max(0, min(location - row_count // 2, len(available) - row_count))
        for index in available[start:start + row_count]:
            result = runner.results[index]
            marker = '▶' if index == self.selected else ' '
            seconds = time.monotonic() - result.started if result.status == 'running' else result.seconds
            reports = result.passed + result.failed + result.skipped
            progress = f'{reports}/{result.total}' if result.total else f'{reports} reports' if reports else 'complete' if result.status == 'passed' else ''
            text = f'{marker} {result.status.upper():9} {result.stage.name:34} {seconds:6.1f}s  {progress}'
            if result.failed:
                text += f'  {result.failed} failed checks'
            lines.append((text, '31' if result.failed else COLOURS[result.status]))
        if not available:
            lines.append(('No failed gates yet.', '32'))
        result = runner.results[min(self.selected, total - 1)] if total else None
        if result:
            lines.append(('─ DETAILS  ' + result.stage.name + '  |  PgUp/PgDn scroll  l full log ' + '─' * width, '90'))
            show_failures = result.failures and not self.full_log and (result.status in BAD or self.failures_only)
            lines.append((f'{len(result.failures)} failing cases' if show_failures else result.current or result.detail or 'Waiting to start', '36'))
            log_lines = list(result.tail)
            if show_failures:
                log_lines = '\n\n'.join(result.failures).splitlines()
            elif result.status != 'running' and result.log:
                path = runner.folder / result.log
                if path.is_file():
                    log_lines = path.read_text(encoding='utf-8', errors='replace').splitlines()
            available_height = max(0, height - len(lines) - 3)
            self.offset = min(self.offset, max(0, len(log_lines) - available_height))
            end = max(0, len(log_lines) - self.offset)
            for line in log_lines[max(0, end - available_height):end]:
                colour = '31' if line.startswith(('[FAIL]', 'Traceback', 'AssertionError')) else '0'
                lines.append((line, colour))
        while len(lines) < height - 3:
            lines.append(('', '0'))
        lines.append(('─' * width, '90'))
        lines.append(('FAILED — Enter/q to exit with failure status' if self.finished else 'Running — q/Ctrl-C cancels the process tree; success exits automatically', '31' if self.finished else '32'))
        lines.append(('Logs: ' + str(runner.folder), '90'))
        return [(fit(text, width), colour) for text, colour in lines[:height]]

    def draw(self, runner, force=False):
        now = time.monotonic()
        if not force and now - self.last_draw < 0.1:
            return
        self.last_draw = now
        size = shutil.get_terminal_size((100, 30))
        frame = self.frame(runner, max(1, size.columns - 1), max(1, size.lines - 1))
        sys.stdout.write('\x1b[H' + '\n'.join(f'\x1b[{colour}m{text}\x1b[0m' for text, colour in frame) + '\x1b[J')
        sys.stdout.flush()

    def finish(self, runner, pause=True):
        if not self.terminal.enabled or not pause or runner.cancelled or not any(r.status in BAD for r in runner.results):
            return
        self.finished = True
        self.follow = False
        self.failures_only = True
        self.selected = next(i for i, r in enumerate(runner.results) if r.status in BAD)
        self.draw(runner, force=True)
        while True:
            incoming = self.terminal.poll()
            if any(key in {'enter', 'q', 'escape', 'cancel'} for key in incoming):
                break
            self.handle_keys(runner, incoming)
            self.draw(runner)
            time.sleep(0.05)
