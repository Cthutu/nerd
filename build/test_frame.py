#!/usr/bin/env python3
"""Exercise Linux frame transitions against a deterministic Xlib peer."""
import argparse
import os
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--nerd', default='_bin/nerd-debug')
    args = parser.parse_args()
    if not sys.platform.startswith('linux'):
        print('Frame X11 regression skipped: Linux only')
        return
    compiler = Path(args.nerd).resolve()
    env = os.environ.copy()
    env['NERD_LIB_PATH'] = str(ROOT / 'mods')
    scratch = ROOT / '_tmp'
    scratch.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='frame-', dir=scratch) as directory:
        directory = Path(directory)
        peer = directory / 'libframe-peer.so'
        executable = directory / 'frame-test'
        subprocess.run(['clang', '-shared', '-fPIC', str(ROOT / 'tests/ffi/frame_x11.c'),
                        '-o', str(peer)], check=True, timeout=30)
        source = directory / 'main.n'
        source.write_text(
            f'build {{ library_path: "{directory}" }}\n'
            + (ROOT / 'tests/frame/fixed-fullscreen.n').read_text(encoding='utf-8'),
            encoding='utf-8')
        # Link the peer so the probe can inspect hints; preload also interposes Xlib.
        subprocess.run([str(compiler), 'build', '-o', str(executable),
                        str(source)],
                       env=env, check=True, timeout=60)
        env['LD_PRELOAD'] = str(peer)
        env['LD_LIBRARY_PATH'] = str(directory)
        subprocess.run([str(executable)], env=env, check=True, timeout=15)
    print('Frame X11 fullscreen regression passed')


if __name__ == '__main__':
    main()
