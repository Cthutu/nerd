# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `a618456b5d0013917b22a50933f9408cedfd5dbb`
Branch: `experiment/task-scheduler-performance`

Selection: ['toolchain-debug', 'toolchain-release', 'cgen-debug', 'cgen-release']
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 2.367 | [build-debug.log](build-debug.log) |
| build-release | passed | 4.739 | [build-release.log](build-release.log) |
| toolchain-debug | failed | 7.893 | [toolchain-debug.log](toolchain-debug.log) |
| cgen-debug | passed | 96.622 | [cgen-debug.log](cgen-debug.log) |
| toolchain-release | passed | 14.039 | [toolchain-release.log](toolchain-release.log) |
| cgen-release | passed | 96.361 | [cgen-release.log](cgen-release.log) |
