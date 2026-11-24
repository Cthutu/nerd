#!/usr/bin/env python3
"""Check terminal shutdown with queued and in-flight protocol reports on Linux."""
import argparse
import os
from pathlib import Path
import select
import shlex
import subprocess
import sys
import tempfile
import threading
import time

ROOT = Path(__file__).resolve().parents[1]
REPORTS = b"\x1b[13;1:3u\x1b[<0;10;5m\x1b[?31u\x1b[?1;2c"


def lifecycle(binary, env, delivery):
    import fcntl
    import pty
    import struct
    import termios

    master, slave = pty.openpty()
    original = termios.tcgetattr(slave)
    proc = None
    output = bytearray()
    sent = False
    delayed = delivery == "delayed"
    continuous = delivery == "continuous"
    late = not delayed
    stop = threading.Event()
    producer = None
    quit_started = None
    def reports():
        while not stop.is_set():
            try:
                os.write(master, REPORTS)
            except BlockingIOError:
                pass
            except OSError:
                return
            stop.wait(0.001)
    try:
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 24, 80, 0, 0))
        proc = subprocess.Popen([str(binary)], stdin=slave, stdout=slave, stderr=slave,
                                env=env, cwd=ROOT, start_new_session=True)
        deadline = time.monotonic() + 5
        while time.monotonic() < deadline:
            if select.select([master], [], [], 0.01)[0]:
                output.extend(os.read(master, 65536))
            if not sent and b"\x1b[?1006h" in output:
                quit_started = time.monotonic()
                os.write(master, b"q" if delayed or continuous else b"q" + REPORTS * 16)
                if continuous:
                    os.set_blocking(master, False)
                    producer = threading.Thread(target=reports, daemon=True)
                    producer.start()
                sent = True
            if delayed and not late and b"\x1b[<u" in output:
                time.sleep(0.01)
                os.write(master, REPORTS)
                late = True
            if proc.poll() is not None:
                break
        assert sent and late, ("missing protocol lifecycle", bytes(output))
        assert proc.wait(timeout=1) == 0, bytes(output)
        stop.set()
        if producer is not None:
            producer.join(timeout=1)
            assert not producer.is_alive()
            assert time.monotonic() - quit_started < 1, "continuous input prevented bounded shutdown"
        assert termios.tcgetattr(slave) == original, "terminal mode was not restored"
        assert b"\x1b[?1006l" in output and b"\x1b[<u" in output and b"\x1b[?1049l" in output
        if continuous:
            return  # Bytes sent after mode restoration belong to the next reader.
        # A real shell would read this same slave descriptor after the child.
        # Fresh input must arrive without any old key/mouse/negotiation reports.
        os.write(master, b"shell-input\n")
        assert select.select([slave], [], [], 1)[0], "fresh shell input disappeared"
        leftover = os.read(slave, 4096)
        assert leftover == b"shell-input\n", f"terminal reports leaked to shell: {leftover!r}"
    finally:
        stop.set()
        if producer is not None:
            producer.join(timeout=1)
        if proc is not None and proc.poll() is None:
            proc.kill()
            proc.wait()
        os.close(master)
        os.close(slave)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nerd", type=Path, required=True)
    args = parser.parse_args()
    if not sys.platform.startswith("linux"):
        print("SKIP terminal shutdown PTY checks require Linux")
        return
    env = {**os.environ, "NERD_LIB_PATH": str(ROOT / "mods"), "TERM": "xterm-256color"}
    with tempfile.TemporaryDirectory(prefix="nerd-term-") as directory:
        for backend, flags in [("LLVM", []), ("C", ["--cgen"])]:
            for mode, optimisation in [("debug", []), ("release", ["--release"])]:
                binary = Path(directory) / f"term-{backend}-{mode}"
                command = [str(args.nerd.resolve()), "build", *flags, *optimisation,
                           "-o", str(binary), str(ROOT / "tests/term/lifecycle.n")]
                if backend == "C":
                    options = subprocess.check_output([*command, "--copts"], env=env, cwd=ROOT, text=True)
                    subprocess.run(["clang", str(binary.with_suffix(".c")), *shlex.split(options),
                                    "-o", str(binary)], env=env, cwd=ROOT, check=True)
                else:
                    subprocess.run(command, env=env, cwd=ROOT, check=True)
                for delivery in ("queued", "delayed", "continuous"):
                    lifecycle(binary, env, delivery)
                print(f"PASS {backend} {mode}: queued/in-flight reports, bounded shutdown, mode restoration and fresh shell input", flush=True)


if __name__ == "__main__":
    main()
