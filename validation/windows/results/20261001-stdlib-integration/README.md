# Standard-library integration — 2026-10-01

Review branch: `std-library`, targeting `main` through a draft PR. This combines
the first capability, runtime, threading and IPv4 socket slices, with compiler
repairs found during their validation. It does not implement the Raptor scheduler,
Kerberos graphs or Nexus protocols, nor declare M0/M1/M5 complete.

## Changes and review map

- [Workstreams](../../../../docs/stdlib-workstreams.md) preserves branch ownership
  and the integration sequence; published commits are retained by merges.
- [Thread/sync contracts](../../../../docs/stdlib-thread-sync.md) describe callback
  context lifetime, join ownership and predicate-based waits.
- [Network contracts](../../../../docs/std-network-foundation.md) describe socket
  ownership, partial I/O, EOF, nonblocking operations and datagram truncation.
- `thread-pipeline`, `network-echo` and `network-datagram` are bounded runnable
  examples included in the execution tests. Examples use native resources rather
  than sleeps or probed-then-released ports.
- The C-output constant repair preserves per-use evaluation and isolates imported
  binders. Runtime worker cleanup and debug allocation bookkeeping are integrated.
- Both `just test` and `just test-release` run the new library contracts; the OS
  completion fixture includes both `socket` and `thread`.

## Validation scope

Fresh native Windows Clang builds and the combined debug/release recipes are the
integration gate. Exact final results are recorded below. These
checks use checkout modules and compilers. The common recipe includes isolated
clean and installation smoke tests; it does not run the global compiler/editor
installation performed by `just do`. Desktop/editor observations, benchmarks and
global installation are outside this review pass.

Final Arch WSL snapshot `2b540cd91c3e3bd1dde64b62ae9e5bd1169b91f4` passed a fresh Clang
22.1.6 native compiler build, runtime C worker stress (debug/release, three runs
each), thread/sync C output at O0/O2 including the temporary-arena alias regression,
and socket layouts/contracts/examples at O0/O2. The strengthened bit-field and
optional-void regressions also passed at O0/O2. See [the final WSL log](wsl-arch-final.log);
[earlier evidence](wsl-arch.log) at `2ceff08d` is retained separately.
`nerd doctor` reports missing `opt`, `llc`, `ld.lld` and `llvm-ar` in that
WSL environment. No tool installation was performed. This is generated-C WSL
evidence, not native LLVM or standalone Linux adoption.

## Integration failure history

The first full Windows fixture run at `5d0ade99` had 1173 passes, eight failures
and 15 platform skips. Six failures were diagnostic/LSP source locations shifted
by the new core accessor; updating only those coordinates passes the focused
reruns. The two real failures were previously hidden LLVM lowering gaps in
pointer bit-field assignment and optional-void constructors. The stricter error
path correctly exposed them instead of silently ending generated functions.

Repair `3e32a7e0` fixes both lowerers and strengthens fixtures 281/326 with
completion/side-effect assertions. That also exposed and fixed C-output dropping
void-expression side effects. Independent review and focused LLVM/C O0/O2 checks
passed before integration. [Initial full failure log](just-test-initial-failed.log)
and [C-output preflight](cgen-preflight.log) preserve the failure evidence; they
are superseded by the final combined runs below.

## Final results

Implementation revision: **`2b540cd91c3e3bd1dde64b62ae9e5bd1169b91f4`**. Later
commits contain evidence/documentation and reviewed whitespace-only C formatting.

| Check | Result | Evidence |
| --- | --- | --- |
| Native Windows `just test` | Passed: 1181 fixtures, 0 failures, 15 platform skips; all auxiliary checks passed | [Full log](just-test.log) |
| Native Windows `just test-release` | Passed: 1181 fixtures, 0 failures, 15 platform skips; release-compiler capability/thread/network gates passed | [Full log](just-test-release.log) |
| Generated-C differential gate | 298 fixtures at O0/O2 passed, 2 platform skips; Linux PTY check skipped on Windows | Included in debug recipe log |
| New runtime/thread/network gates | Runtime stress, ABI checks, arena aliasing, lifecycle/predicate waits, socket contracts and all three examples passed | Both recipe logs |
| Arch WSL bounded C-output gate | Passed on final implementation revision, including strengthened 281/326 | [Final WSL log](wsl-arch-final.log) |
| `just format` | Passed; retained touched compiler formatting, excluded unrelated pre-existing module drift | [Format log](just-format.log) |

The debug recipe also passed 6000-term depth, ordered/concurrent front-end and
LLVM, profiling, toolchain, temporary installation, formatter workflow, source
build settings and all four Windows stdio configurations. No tests were weakened
or skipped to hide the integration failures. Historical failing logs remain
explicitly labelled above. `main` is unchanged, and no global compiler/editor
installation was performed. Full `just do`, desktop/benchmark validation and
native Linux LLVM adoption remain separate gates.

## Linux pickup

Use the remote name configured on the Linux PC (`git remote -v`), fetch and check
out `std-library`, then fast-forward it from that remote. Review any existing
local edits before switching or pulling. No integration into `main` is required
to review this draft.

Ensure recent Clang with C23 `#embed`, `opt`, `llc`, LLD, LLVM archive/debug tools,
Python, `uv` and `just` are available. The initial pthread and socket bindings are
for **Linux x86-64 glibc**; their native header assertions reject incompatible
layouts rather than treating them as tested platforms.

```sh
just test
just test-release
just run-example thread-pipeline
just run-example network-echo
just run-example network-datagram
```

Those commands build this checkout's compiler and use its modules. The two test
recipes include capabilities, runtime worker stress (debug recipe), thread/sync,
networking and the imported-arena regression. The debug recipe additionally runs
the full C-output differential suite. Run `just do` when ready to exercise the
clean/build/test/global-install workflow on that PC; it intentionally updates the
installed compiler/modules and editor support. Native Linux/full WSL `just do`
remain adoption gates and are not implied by the Windows results.

Source access was resolved during integration. The
[inventory](../../../../docs/stdlib-source-inventory.md) pins Raptor/Kerberos
revisions, tests and provenance; upstream execution and reuse decisions remain.
No work-stealing implementation was found in the available Raptor heads, so
confirm the intended revision or explicitly choose an algorithm. Continue the
[milestone plan](../../../../docs/stdlib-expansion-plan.md)
after reviewing this foundation: queues/scheduler, graph semantics, broader socket
support and Nexus framing/protocols remain separate work.
