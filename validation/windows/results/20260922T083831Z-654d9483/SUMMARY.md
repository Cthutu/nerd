# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `828d4b56ec08ea7f2001334e33cf660da9d82bbb`
Branch: `experiment/task-scheduler-performance`

Selection: full
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 1.402 | [build-debug.log](build-debug.log) |
| build-release | passed | 1.423 | [build-release.log](build-release.log) |
| clean | passed | 0.439 | [clean.log](clean.log) |
| memory | passed | 2.274 | [memory.log](memory.log) |
| threads | passed | 4.132 | [threads.log](threads.log) |
| fixtures | passed | 177.209 | [fixtures.log](fixtures.log) |
| doctor-debug | passed | 0.221 | [doctor-debug.log](doctor-debug.log) |
| profile-debug | passed | 1.123 | [profile-debug.log](profile-debug.log) |
| render-threads-debug | passed | 3.16 | [render-threads-debug.log](render-threads-debug.log) |
| jobs-debug | passed | 16.423 | [jobs-debug.log](jobs-debug.log) |
| front-threads-debug | failed | 33.764 | [front-threads-debug.log](front-threads-debug.log) |
| toolchain-debug | passed | 17.838 | [toolchain-debug.log](toolchain-debug.log) |
| install-debug | failed | 0.663 | [install-debug.log](install-debug.log) |
| cgen-debug | failed | 23.023 | [cgen-debug.log](cgen-debug.log) |
| debugger-debug | passed | 1.102 | [debugger-debug.log](debugger-debug.log) |
| editor-debug | passed | 0.231 | [editor-debug.log](editor-debug.log) |
| doctor-release | passed | 0.242 | [doctor-release.log](doctor-release.log) |
| profile-release | passed | 0.92 | [profile-release.log](profile-release.log) |
| render-threads-release | passed | 1.987 | [render-threads-release.log](render-threads-release.log) |
| jobs-release | passed | 13.385 | [jobs-release.log](jobs-release.log) |
| front-threads-release | failed | 1.591 | [front-threads-release.log](front-threads-release.log) |
| toolchain-release | passed | 14.63 | [toolchain-release.log](toolchain-release.log) |
| install-release | failed | 0.552 | [install-release.log](install-release.log) |
| cgen-release | failed | 23.027 | [cgen-release.log](cgen-release.log) |
| debugger-release | passed | 1.001 | [debugger-release.log](debugger-release.log) |
| editor-release | passed | 0.185 | [editor-release.log](editor-release.log) |
| adapter-transforms | passed | 3.338 | [adapter-transforms.log](adapter-transforms.log) |
| benchmark | blocked |  | Prerequisites failed: front-threads-debug, install-debug, cgen-debug, front-threads-release, install-release, cgen-release |
