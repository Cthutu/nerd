# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `abee4399fd3cdd719e49e2ae06c06c72888840f5`
Branch: `experiment/task-scheduler-performance`

Selection: ['cgen-debug', 'cgen-release', 'front-threads-debug', 'front-threads-release']
Automated status: **passed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 1.815 | [build-debug.log](build-debug.log) |
| build-release | passed | 2.195 | [build-release.log](build-release.log) |
| front-threads-debug | passed | 86.108 | [front-threads-debug.log](front-threads-debug.log) |
| cgen-debug | passed | 91.893 | [cgen-debug.log](cgen-debug.log) |
| front-threads-release | passed | 80.229 | [front-threads-release.log](front-threads-release.log) |
| cgen-release | passed | 91.603 | [cgen-release.log](cgen-release.log) |
