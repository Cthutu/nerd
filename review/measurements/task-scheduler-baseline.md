# Task scheduler exploratory baseline

Measured 2026-09-19 on source commit `a15e965dcfd4c554cc7691a82786142f74031eab`,
with no compiler modifications. `just build-release nerd --skip-mod-sync`
confirmed the release compiler was current. Linux x86-64, AMD Ryzen 9 7950X,
16 physical cores / 32 logical CPUs, Clang 22.1.8. CPU boost enabled; affinity,
background load and frequency were not controlled. No parallel benchmark runs.

The [audit and milestone plan](../audits/task-scheduler-performance.md) interprets
these results. [results.json](task-scheduler-baseline/results.json) contains
exact argv, all five measured wall times and medians. Adjacent text files contain
the final sample's complete timing report, with ANSI escapes removed. These
reports are individual samples, not phase medians. The compiler's Linux front-end
clock is thread CPU time; backend phases use wall time. End-to-end measurements
below use Python's monotonic performance clock and include process startup.

Run this from the repository root after building the release compiler:

```python
import json
import os
from pathlib import Path
import statistics
import subprocess
import time

out = Path('/tmp/nerd-scheduler-audit')
out.mkdir(exist_ok=True)
(out / 'tiny.n').write_text('main :: fn () -> i32 { return 0 }\n')
env = dict(os.environ, NERD_LIB_PATH=str(Path('mods').resolve()))
rows = []
inputs = [
    ('tiny', str(out / 'tiny.n')),
    ('dungeon', 'examples/dungeon/dungeon.n'),
    ('pixels', 'examples/pixels/pixels.n'),
    ('quill', 'examples/text-adventure/quill.n'),
]
for name, source in inputs:
    for mode in ['check', 'llvm', 'cgen']:
        cmd = ['_bin/nerd', '--timing',
               'check' if mode == 'check' else 'build']
        if mode == 'cgen':
            cmd += ['--cgen']
        if mode != 'check':
            cmd += ['-o', str(out / (name + '-' + mode))]
        cmd += [source]
        times = []
        for iteration in range(6):
            start = time.perf_counter()
            result = subprocess.run(cmd, env=env, capture_output=True, text=True)
            elapsed = (time.perf_counter() - start) * 1000
            if result.returncode:
                raise RuntimeError(result.stdout + result.stderr)
            if iteration:  # Discard warm-up.
                times.append(elapsed)
        (out / (name + '-' + mode + '.txt')).write_text(
            result.stdout + result.stderr)
        rows.append(dict(name=name, mode=mode, command=cmd,
                         milliseconds=times, median=statistics.median(times)))
(out / 'results.json').write_text(json.dumps(rows, indent=2))
```

All 72 invocations succeeded. Outputs were written under `/tmp`, leaving example
artifacts unchanged. No generated programs were run. C generation excludes Clang;
LLVM builds include Clang. All target builds used the default debug configuration,
despite using a release compiler executable. No `--llvm` sidecars were requested.

This small sample establishes opportunities, not scaling or causation. M1 should
add target release builds, pinned single-core comparisons, synthetic dependency
graphs, module-labelled CPU profiles, cold-cache methodology, memory accounting,
and enough repetitions to estimate noise before setting performance gates.
