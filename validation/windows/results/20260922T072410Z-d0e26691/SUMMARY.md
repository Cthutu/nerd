# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `6bbc17e8e14facc95caee55abdca35db216bbb05`
Branch: `experiment/task-scheduler-performance`

Selection: full
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 16.573 | [build-debug.log](build-debug.log) |
| build-release | passed | 27.164 | [build-release.log](build-release.log) |
| clean | passed | 0.41 | [clean.log](clean.log) |
| memory | passed | 2.253 | [memory.log](memory.log) |
| threads | passed | 4.0 | [threads.log](threads.log) |
| fixtures | failed | 150.403 | [fixtures.log](fixtures.log) |
| doctor-debug | passed | 0.205 | [doctor-debug.log](doctor-debug.log) |
| profile-debug | failed | 0.628 | [profile-debug.log](profile-debug.log) |
| render-threads-debug | passed | 3.007 | [render-threads-debug.log](render-threads-debug.log) |
| jobs-debug | failed | 14.346 | [jobs-debug.log](jobs-debug.log) |
| front-threads-debug | passed | 88.634 | [front-threads-debug.log](front-threads-debug.log) |
| toolchain-debug | passed | 3.339 | [toolchain-debug.log](toolchain-debug.log) |
| install-debug | failed | 0.349 | [install-debug.log](install-debug.log) |
| cgen-debug | failed | 84.789 | [cgen-debug.log](cgen-debug.log) |
| debugger-debug | passed | 2.295 | [debugger-debug.log](debugger-debug.log) |
| doctor-release | passed | 0.22 | [doctor-release.log](doctor-release.log) |
| profile-release | failed | 0.754 | [profile-release.log](profile-release.log) |
| render-threads-release | passed | 1.933 | [render-threads-release.log](render-threads-release.log) |
| jobs-release | failed | 11.212 | [jobs-release.log](jobs-release.log) |
| front-threads-release | failed | 78.41 | [front-threads-release.log](front-threads-release.log) |
| toolchain-release | passed | 3.231 | [toolchain-release.log](toolchain-release.log) |
| install-release | failed | 0.358 | [install-release.log](install-release.log) |
| cgen-release | failed | 84.823 | [cgen-release.log](cgen-release.log) |
| debugger-release | passed | 0.995 | [debugger-release.log](debugger-release.log) |
| benchmark | blocked |  | Prerequisites failed: fixtures, profile-debug, jobs-debug, install-debug, cgen-debug, profile-release, jobs-release, front-threads-release, install-release, cgen-release |
