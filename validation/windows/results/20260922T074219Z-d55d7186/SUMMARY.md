# Windows validation run

Platform: Windows-11-10.0.26200-SP0
Commit at start: `c760a1bf42c9fa5cc7a9766e944385b292bd931c`
Branch: `experiment/task-scheduler-performance`

Selection: full
Automated status: **incomplete-or-failed**. Manual checks: see MANUAL.md.

| Stage | Status | Seconds | Log / reason |
| --- | --- | ---: | --- |
| build-debug | passed | 1.446 | [build-debug.log](build-debug.log) |
| build-release | passed | 1.416 | [build-release.log](build-release.log) |
| clean | passed | 0.408 | [clean.log](clean.log) |
| memory | passed | 2.209 | [memory.log](memory.log) |
| threads | passed | 4.066 | [threads.log](threads.log) |
| fixtures | passed | 147.512 | [fixtures.log](fixtures.log) |
| doctor-debug | passed | 0.206 | [doctor-debug.log](doctor-debug.log) |
| profile-debug | passed | 0.797 | [profile-debug.log](profile-debug.log) |
| render-threads-debug | passed | 3.01 | [render-threads-debug.log](render-threads-debug.log) |
| jobs-debug | passed | 14.688 | [jobs-debug.log](jobs-debug.log) |
| front-threads-debug | passed | 86.64 | [front-threads-debug.log](front-threads-debug.log) |
| toolchain-debug | passed | 8.423 | [toolchain-debug.log](toolchain-debug.log) |
| install-debug | passed | 1.85 | [install-debug.log](install-debug.log) |
| cgen-debug | passed | 91.331 | [cgen-debug.log](cgen-debug.log) |
| debugger-debug | passed | 0.934 | [debugger-debug.log](debugger-debug.log) |
| doctor-release | passed | 0.217 | [doctor-release.log](doctor-release.log) |
| profile-release | passed | 0.815 | [profile-release.log](profile-release.log) |
| render-threads-release | passed | 1.97 | [render-threads-release.log](render-threads-release.log) |
| jobs-release | passed | 11.853 | [jobs-release.log](jobs-release.log) |
| front-threads-release | failed | 36.761 | [front-threads-release.log](front-threads-release.log) |
| toolchain-release | passed | 8.151 | [toolchain-release.log](toolchain-release.log) |
| install-release | passed | 1.846 | [install-release.log](install-release.log) |
| cgen-release | passed | 88.354 | [cgen-release.log](cgen-release.log) |
| debugger-release | passed | 0.956 | [debugger-release.log](debugger-release.log) |
| benchmark | blocked |  | Prerequisites failed: front-threads-release |
