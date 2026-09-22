#!/usr/bin/env python3
"""Check benchmark accounting against a child with known CPU and memory work."""
import os
import sys

from benchmark_compiler import invoke

sample, records = invoke([sys.executable, '-c', '''
import time
memory = bytearray(32 * 1024 * 1024)
start = time.process_time()
while time.process_time() - start < 0.1:
    memory[0] = (memory[0] + 1) % 256
'''], dict(os.environ))
assert not records
assert sample['wall_ns'] > 0
if os.name == 'nt' or hasattr(os, 'wait4'):
    assert sample['user_seconds'] + sample['system_seconds'] >= 0.08, sample
    assert sample['max_process_rss_bytes'] >= 32 * 1024 * 1024, sample
    assert sample['accounting_scope'] != 'unavailable', sample
    print('Known child CPU and peak-memory accounting passed:', sample)
else:
    print('[SKIP] native process accounting unavailable:', sample)
