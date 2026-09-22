# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `66ad592e1e210ed514bde6573903627b9eaaba88`
Branch: `experiment/task-scheduler-performance`

Selection: ['profile-debug', 'profile-release', 'jobs-debug', 'jobs-release', 'toolchain-debug', 'toolchain-release', 'install-debug', 'install-release', 'cgen-debug', 'cgen-release', 'front-threads-release']
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 12.24 | [build-debug.log](build-debug.log) |
| build-release | passed | 23.984 | [build-release.log](build-release.log) |
| profile-debug | passed | 0.832 | [profile-debug.log](profile-debug.log) |
| jobs-debug | passed | 14.648 | [jobs-debug.log](jobs-debug.log) |
| toolchain-debug | passed | 7.872 | [toolchain-debug.log](toolchain-debug.log) |
| install-debug | passed | 1.857 | [install-debug.log](install-debug.log) |
| cgen-debug | passed | 93.599 | [cgen-debug.log](cgen-debug.log) |
| profile-release | passed | 0.792 | [profile-release.log](profile-release.log) |
| jobs-release | passed | 11.977 | [jobs-release.log](jobs-release.log) |
| front-threads-release | failed | 1.988 | [front-threads-release.log](front-threads-release.log) |
| toolchain-release | passed | 7.379 | [toolchain-release.log](toolchain-release.log) |
| install-release | passed | 1.834 | [install-release.log](install-release.log) |
| cgen-release | passed | 90.373 | [cgen-release.log](cgen-release.log) |
