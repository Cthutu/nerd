# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `66ad592e1e210ed514bde6573903627b9eaaba88`
Branch: `experiment/task-scheduler-performance`

Selection: ['front-threads-release', 'toolchain-debug', 'toolchain-release']
Automated status: **passed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 1.447 | [build-debug.log](build-debug.log) |
| build-release | passed | 1.451 | [build-release.log](build-release.log) |
| toolchain-debug | passed | 8.452 | [toolchain-debug.log](toolchain-debug.log) |
| front-threads-release | passed | 79.593 | [front-threads-release.log](front-threads-release.log) |
| toolchain-release | passed | 7.832 | [toolchain-release.log](toolchain-release.log) |
