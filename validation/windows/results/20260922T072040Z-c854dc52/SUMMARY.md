# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `6bbc17e8e14facc95caee55abdca35db216bbb05`
Branch: `experiment/task-scheduler-performance`

Selection: full
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | failed | 9.63 | [build-debug.log](build-debug.log) |
| build-release | failed | 11.86 | [build-release.log](build-release.log) |
| clean | passed | 0.496 | [clean.log](clean.log) |
| memory | passed | 2.218 | [memory.log](memory.log) |
| threads | passed | 3.95 | [threads.log](threads.log) |
| fixtures | blocked |  | Prerequisites failed: build-debug |
| doctor-debug | blocked |  | Prerequisites failed: build-debug |
| profile-debug | blocked |  | Prerequisites failed: build-debug |
| render-threads-debug | blocked |  | Prerequisites failed: build-debug |
| jobs-debug | blocked |  | Prerequisites failed: build-debug |
| front-threads-debug | blocked |  | Prerequisites failed: build-debug |
| toolchain-debug | blocked |  | Prerequisites failed: build-debug |
| install-debug | blocked |  | Prerequisites failed: build-debug |
| cgen-debug | blocked |  | Prerequisites failed: build-debug |
| debugger-debug | blocked |  | Prerequisites failed: build-debug |
| doctor-release | blocked |  | Prerequisites failed: build-release |
| profile-release | blocked |  | Prerequisites failed: build-release |
| render-threads-release | blocked |  | Prerequisites failed: build-release |
| jobs-release | blocked |  | Prerequisites failed: build-release |
| front-threads-release | blocked |  | Prerequisites failed: build-release |
| toolchain-release | blocked |  | Prerequisites failed: build-release |
| install-release | blocked |  | Prerequisites failed: build-release |
| cgen-release | blocked |  | Prerequisites failed: build-release |
| debugger-release | blocked |  | Prerequisites failed: build-release |
| benchmark | blocked |  | Prerequisites failed: build-debug, build-release, fixtures, doctor-debug, profile-debug, render-threads-debug, jobs-debug, front-threads-debug, toolchain-debug, install-debug, cgen-debug, debugger-debug, doctor-release, profile-release, render-threads-release, jobs-release, front-threads-release, toolchain-release, install-release, cgen-release, debugger-release |
