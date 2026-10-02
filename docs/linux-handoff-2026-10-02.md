# Linux pickup — 2 October 2026

Continue standard-library work on **`std-library`**, whose latest implementation
commit is **`6bc00639`**. This is the main working branch for this project, not
the repository's `main`. [Draft PR #1](https://github.com/Cthutu/nerd/pull/1)
still targets `main`. This handoff is a documentation-only follow-up.

## Decisions from today's review

- Nerd is a C replacement with better syntax, generics, traits and slices. Do
  not introduce Rust-style ownership, borrowing, pinning or transfer traits as
  requirements. Allocation, aliasing, cleanup and synchronization are explicit
  programmer/API responsibilities.
- Kerberos should be a lightweight Grand Central Dispatch-style system. ASIO
  was an implementation detail, not an architectural goal or required dependency.
- Ordinary plex layout belongs to the compiler: fields may be reordered or padded
  for space and performance. Preserve correct alignment and named field access.
  `#c` preserves the target C ABI; existing `#packed` implies packed C layout.
  Current source-order lowering for ordinary plexes is not a language guarantee.
- Untyped integer literals must acquire a suitable contextual type without an
  explicit cast and must fit that destination. These are compiler correctness
  requirements, not optional convenience features.
- Investigate and fix compiler bugs immediately when discovered. Add concrete
  examples to recommendations. Commit verified work and **always push commits**.
- Test consolidation should reduce compiler/toolchain launches while preserving
  coverage. Group compatible runtime scenarios by feature with exact labeled
  output, retain distinct IR/CLI/ABI/platform checks, and assert the intended
  diagnostic for invalid programs. Fewer files alone is not the success measure.

## Today's work on `std-library`

| Commits | Result |
| --- | --- |
| `6a93d32b`, `915d18c0` | Added/revised the [review report](stdlib-review-report.md), corrected the C-replacement and Kerberos design framing, and added concrete code examples. Updated the milestone/source documents and draft PR description. |
| `e16538a6`, `e6dd9f01` | Typed FFI callbacks: existing function types are accepted for compatible native signatures. Windows/Linux thread bindings use `NativeThreadEntry` without raw-pointer casts. Recursive checks reject unsupported callback aggregates; no new syntax or trampoline. Second commit normalizes fixture/log line endings. |
| `c861db21` | Fixed `count: atomic[usize] = 7`: inference sees the element type before defaulting the literal. Capability probe no longer uses a cast; typed-value compatibility and atomic operations are unchanged. |
| `dd942510` | Documented compiler-controlled ordinary plex layout. Added atomic alignment/access probes for stack/nested plexes, fixed/dynamic arrays, arena and heap storage, plus native C comparison of a `#c` record's size/offsets. No field-reordering optimizer or new layout annotation was implemented. |
| `6bc00639` | Semantic destination-range checking for literals, including atomic and default-inferred storage. Preserves signed minima, full unsigned 64-bit magnitudes and explicit narrowing. Also fixes premature i32 truncation of large literals before explicit casts. Intentional signed bit patterns and Windows unsigned handle constants now use explicit casts. |

Range coverage includes 23 source/diagnostic pairs in
[`141-integer-literal-range.e`](../tests/errors/141-integer-literal-range.e),
extensions to existing command 349, and the existing C differential integer-
wrapping fixture. It covers named constants, arguments, returns, fields,
assignments and default scalar/array/tuple storage. Ordinary concrete integer
arithmetic retains its existing wrapping behavior. No fixture harness was
changed on this branch during these compiler fixes.

Natural alignment probes pass for the tested Windows x64 configurations; they
do not establish cache-line separation, packed-atomic safety or lock-free queue
correctness. Arena alignment currently derives from size and is capped at eight
bytes; `std.memory.alloc` guarantees 16 bytes. Defer cache-layout policy or a new
separation/alignment facility until queue measurements demonstrate a need.

## Evidence and limits

- Latest implementation `6bc00639`: native Windows fixture suite **1193 passed,
  zero failures, 15 platform skips**; **300 LLVM/C differential fixtures** passed
  at C O0/O2, two platform skips. Fresh release compiler passes range diagnostics,
  14 atomic-related fixtures and LLVM capability/layout probes. See
  [full results and retained failure history](../validation/windows/results/20261002-integer-literal-range/README.md).
- Typed callback work passed native Windows LLVM/C thread checks in debug and
  optimized programs. Arch WSL passed a fresh Clang compiler build and Linux
  thread checks using generated C. See [callback evidence](../validation/windows/results/20261002-typed-ffi-callbacks/README.md).
- Layout/capability probes passed Windows LLVM and generated C in debug and
  optimized configurations. The [runner](../validation/stdlib/check_capabilities.py)
  accepts `--cgen` for explicit C compatibility testing.
- Today's individual compiler changes were checked with the fixture/differential
  suites and focused gates, **not a fresh full `just test`/`just do` on
  `std-library`**. The earlier integrated foundation at `2b540cd9` passed full
  Windows `just test` and `just test-release`; see [integration evidence](../validation/windows/results/20261001-stdlib-integration/README.md).
- Today's complete code has not yet had native Linux or full WSL validation.
  Arch WSL previously lacked `opt`, `llc`, `ld.lld` and `llvm-ar`; passing generated
  C does not replace a direct LLVM run. No global Nerd/editor install, desktop
  validation or benchmarks were performed in today's standard-library work.

## Independent test audit and consolidation

These branches are **pushed but not merged into `std-library`**:

- `test-suite-audit`, `f393eaaa`: [audit report](https://github.com/Cthutu/nerd/blob/test-suite-audit/docs/test-suite-audit.md), reproducible inventory/timing tools and a batching proof. Found 33 exact runtime duplicate command/language pairs, repeated differential baselines, and negative command fixtures without diagnostic assertions. Its range-bug observations describe its historical baseline; `6bc00639` subsequently fixed that bug.
- `test-suite-consolidation`, based on `6bc00639`: audit merged as `007f76a2`;
  compiler fix/batch added as `58818307`; first consolidation pushed as
  `caaef691`. Read its [current report](https://github.com/Cthutu/nerd/blob/test-suite-consolidation/docs/test-suite-consolidation.md)
  and [exact migration map](https://github.com/Cthutu/nerd/blob/test-suite-consolidation/docs/test-suite-consolidation-map.json).

The consolidation agent removed 33 reverified duplicate commands and replaced
14 further `on` commands with one labeled language fixture. Existing HIR/LLVM,
format, LSP, structured-error and distinct CLI checks remain. Twelve differential
command-list entries were replaced by automatic discovery of the batch.
Omission and wrong-result mutations both fail the golden transcript.

Batching exposed another compiler bug: materializing enum payload types could
drop optional/result flags after a structurally equivalent plex was used by a
preceding enum. **Fix `58818307` is currently only on the consolidation branch.**
Preserve it when integrating; its batch covers result and optional forms.

Changed-work timings with the same fixed compiler: the 47 retired commands took
**10.273 s**, the new batch **0.317 s** (medians of three runs after warmup).
Alternative language fixtures remain unchanged and are outside both timings;
these figures are not whole-suite speedups.

At handoff writing, the sub-agent reports full native Windows **`just test`
passed on `caaef691`: 1147 passes, zero failures, 15 skips; 289 differential
fixtures passed, two skips; all auxiliary gates passed**, including 6000-term
depth, toolchain, temporary install and Windows stdio. **`just test-release` is
still running.** The agent will push final evidence and its branch's handoff.
Fetch and read those before claiming completion or integrating the branch.
Coordinate with that agent before editing/pushing the same branch from Linux.

## Linux pickup procedure

1. Inspect the Linux working tree and configured remotes; preserve local edits.
   Fetch and fast-forward `std-library` from its actual upstream. Windows uses
   remote `hub`; do not assume the Linux clone has that name. Read this handoff,
   the [review report](stdlib-review-report.md), [milestone plan](stdlib-expansion-plan.md)
   and [source inventory](stdlib-source-inventory.md).
2. Check prerequisites: Clang to build Nerd, direct LLVM `opt`/`llc` and native
   linker/archive tools for Nerd output, Python, uv and just. Use repository
   compilers/modules, not an older installed Nerd. No Clang fallback in normal
   Nerd binary generation; Clang is used for compiler builds and explicit C tests.
3. Build, inspect the toolchain and run the combined Linux gates:

   ```sh
   just build nerd --skip-mod-sync
   ./_bin/nerd-debug doctor
   just test
   just test-release
   python3 validation/stdlib/check_capabilities.py --compiler _bin/nerd-debug --cgen
   just run-example thread-pipeline
   just run-example network-echo
   just run-example network-datagram
   ```

   Record exact revisions, counts, skips, diagnostics and host/tool versions.
   Fix compiler bugs immediately and preserve regression coverage. Keep native
   Linux, each WSL distribution and Windows evidence distinct.
4. `just do` remains the build/test/install acceptance gate. It starts with
   `just clean` and ends with global installation, including editor integration.
   Preserve needed local artifacts and ensure the machine is ready for those
   effects before invoking it. Do not represent focused checks as a `just do` pass.
5. Fetch the consolidation branch again, check its final evidence, and review the
   mapping/enum fix. Validate on Linux in its own worktree before merging reviewed
   work into `std-library`. Avoid parallel heavy work while timing comparisons.
   Do not silently overwrite the agent's branch or delete worktrees/branches.
6. Update the handoff/results with Linux outcomes, commit and push. Continue
   bounded queue/scheduler and dataflow milestones after the source/algorithm
   decisions below, keeping changes reviewable.

## Remaining standard-library work

Thread/sync and IPv4 TCP/UDP foundations plus `thread-pipeline`, `network-echo`
and `network-datagram` examples are implemented. Raptor work stealing, Kerberos
dispatch/dataflow and Nexus protocols are **not yet ported**. Source access is
resolved, but inspected Raptor heads used ASIO rather than the requested custom
work-stealing implementation. Clarify the intended source revision or select an
algorithm. The old Kerberos executor is serial FIFO dataflow; this does not
override the intended lightweight GCD-style design. Resolve upstream reuse and
attribution before copying implementation material. IPv6, DNS, readiness and
nonblocking-connect completion remain later network slices.

All active task branches were verified backed up on GitHub today. Local `main`
had no unpublished commits and was two commits behind its remote. Two historical
detached worktrees under `C:/Users/matt/projs` contain unrelated uncommitted edits;
they were left untouched and are not part of those branch backups.
