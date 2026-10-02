# Test suite audit

Audit baseline: `std-library` at `c861db2187534ced1d8c94e441f22a69d11f3490`,
2 October 2026. Work is isolated on `test-suite-audit`; no existing fixture,
default recipe, or compiler implementation was changed.

**Follow-up:** [the first consolidation](test-suite-consolidation.md) implements
the duplicate removals and a 14-scenario `on` batch on a separate branch, based
on `std-library` at `6bc00639`. That baseline also fixes the integer destination
range bug described in this historical audit. The original JSON measurements
below remain unchanged. Historical command links identify the audited revision;
the original proof script now reads those fixtures from Git so it remains runnable.

## Recommendation

Consolidate successful runtime tests by feature and remove demonstrably duplicate
command regressions. Keep command tests for command contracts and keep isolated
compiler-error cases. The number to optimise is **compiler/toolchain launches**,
not files or assertions. Many meaningful assertions in one executable are useful.

Start with the **33 exact command/language duplicates** listed below. Preserve
dedicated command cleanup tests, then retire those repeated runtime programs.
Next migrate compatible runtime cases into labelled feature batches, beginning
with `on`. Avoid replacing distinct HIR, LLVM, ABI, platform and CLI assertions
with output-only checks.

A three-case `on` experiment passed its original output/exit expectations, passed
as one executable under LLVM and C at O0/O2, and rejected an intentionally omitted
case. Native Windows LLVM build-and-run time fell from **0.643 s to 0.245 s**
(medians, three repeats after warmup). This is a small proof, not a prediction of
whole-suite speedup.

## Reproduce the evidence

- [Inventory and representative timings](test-suite-audit-data.json).
- [`on` experiment, complete generated source and expected transcript](test-suite-audit-on-batch.json).
- [Inventory tool](../build/audit_tests.py); read-only unless timing is requested.
- [Opt-in batching experiment](../build/audit_on_batch.py); uses temporary files.

```sh
python build/audit_tests.py --output inventory.json
python build/audit_tests.py --measure /path/to/nerd-debug --repeats 3 --limit 5 --output timings.json
python build/audit_on_batch.py --nerd /path/to/nerd-debug --output on-batch.json
```

On this Windows checkout, the measured compiler was
`C:/Users/matt/nerd/_bin/nerd-debug.exe`, built before the audit. Its SHA-256 is
recorded in both evidence files. Every invocation used this worktree's `mods`
and `tests/mods`, not installed modules. No compiler was installed or rebuilt,
and no global environment was changed. Timings ran sequentially; one warmup was
excluded for each fixture/experiment. Windows timings are local measurements;
Linux/WSL measurements and a full-gate baseline remain to be collected. Compiler
build time, cold filesystem/antivirus behaviour and full-suite parallel contention
are outside these measurements.

## What runs today

`build/test.py` discovers **1,207 fixture entries** at this revision. Native
Windows skips 15 (13 commands, two language), giving the previously reported
1,192 passing entries. This is not 1,192 compiler invocations or assertions.

| Family | Entries | Work per entry | Consolidation decision |
| --- | ---: | --- | --- |
| Language | 204 | One Nerd run/build, LLVM toolchain, execution; optional HIR/LLVM assertions | Batch compatible behaviour by feature |
| Errors | 126 | **386 source/expected-diagnostic pairs**, each a separate compiler invocation | Already grouped in files; retain isolated compilation |
| HIR | 28 | One Nerd build, including executable generation, ordered HIR checks | Keep semantic assertions; investigate stopping after HIR emission |
| LLVM | 48 | One Nerd build, including linking, ordered LLVM checks | Keep lowering/debug assertions; investigate emit-only validation |
| Format | 212 | Two formatter invocations: expected text and idempotence | Do not remove idempotence; profile before changing |
| LSP | 212 | One fresh server process, multiple request/response assertions | Distinct protocol and editor behaviour; not replaced by language tests |
| Commands | 359 | Usually one CLI invocation, sometimes additional host/link/artifact/init checks | 33 exact runtime duplicates; retain CLI contracts |
| Standard library | 1 | `nerd test mods/std`; many tests within one reported entry | Already aggregates tests; report inner count separately |
| Examples | 17 | `nerd check` including imported modules | Checks only; not interchangeable with executed example contracts |

The family accounting implies **at least 1,679 top-level Nerd invocations** before
platform skips, or 1,664 on Windows, for a successful fixture pass. This includes
the error subcases and formatter idempotence, but excludes additional commands
inside complex fixtures, subprocesses launched by Nerd, and the auxiliary suites.
It is a static accounting estimate, not a process trace. The fixture harness is
serial; its `--filter` is a path substring, not a subcase filter.

183 language fixtures contain HIR expectations and 164 contain LLVM expectations.
Their sections are ordered subsequences, not necessarily full dumps. Removing a
runtime duplicate does not remove these assertions. Combining all existing
language files without migrating their IR assertions would remove coverage.
The exact-source scan found no identical source between dedicated HIR/LLVM
fixtures and language fixtures; that does not prove semantic non-overlap.

### Auxiliary gates and repeated configurations

`just test` includes considerably more than `build/test.py`:

| Gate | Purpose / relevant cost |
| --- | --- |
| Cleanup | Temporary-workspace cleanup recipe and preservation of source fixtures |
| Memory, runtime threads, compiler threads | Native C allocation/TLS/thread correctness and stress; separate from Nerd language semantics |
| Capability probes | One check plus two builds/runs (debug/release generated programs) |
| Thread/sync | Three programs × two generated-program modes × LLVM/C; native layout compile/run in each backend invocation |
| Network | Three programs × LLVM/C × debug/release; native layout compile/run in each of four invocations |
| Profile | JSON fields, failed phases, escaping, output parity |
| Render threads, expression depth, jobs, front threads | Compiler scheduling/stress/serial-worker parity, including a 6,000-term expression; not normal syntax coverage |
| Toolchain | Direct LLVM tools, missing-tool errors, artifact types, doctor and no Clang fallback |
| Install | Temporary external installation, module resolution, standalone artifacts/debug information |
| Format workflow | Real formatting command/workspace behaviour |
| C generation | Differential runtime matrix, CLI conflicts, `--copts`, host ABI, standalone C, graphics compile and Linux PTY |
| Build settings | Imports, settings ordering, environment paths, defines and build integration |
| Windows stdio | Inherited output and window subsystem integration |

`just test-release` repeats the fixture suite with the **release-built compiler**,
capabilities, both thread backends and release-generated network programs.
`just do` cleans, runs debug and release gates, then installs. A release-built
compiler and an optimised generated program are different dimensions; deleting
either dimension because both say “release” is incorrect.

The repository also has opt-in debugger/adapter/editor checks, benchmark-usage
and semantic-headroom scripts, and validation/benchmark workflows outside these
recipes. They are not represented by the 1,207 fixture count or this timing sample.
Audit those separately before proposing removal. Visible graphics/debugger checks
remain distinct from syntax-only examples.

### Differential testing repeats the LLVM baseline

`build/test_cgen.py` selects 204 language fixtures, 91 command fixtures and seven
C-specific sources: 302 total, 300 active on Windows. Per active fixture it:

1. Builds with LLVM and executes to obtain the reference behaviour.
2. Generates C in a second Nerd invocation.
3. Compiles and executes that C at both O0 and O2.

That is **600 Nerd invocations, 600 Clang invocations and 900 executable launches**
for the Windows differential fixture matrix alone, in a pool of four workers,
excluding extra CLI/graphics/host tests and LLVM's own tool subprocesses.
The LLVM reference uses the default generated-program mode; this is not an
LLVM O0/O2 matrix.

The baseline compilation overlaps the earlier language/selected-command pass.
This is an execution-cost opportunity, not justification for dropping C testing.
A shared orchestrator could build once, validate golden output/HIR/LLVM, then
reuse the observed exit/stdout/stderr for C comparison within that run. It must
preserve path-sensitive diagnostics, stdin, environment, flags and source naming;
several current fixtures depend on those details. Do not cache across compiler
or module changes without an explicit content identity. Standalone C suite use
must retain a self-contained reference build.

Today C differential comparison uses LLVM behaviour as its oracle, not the
fixture's golden exit/stdout. Both backends can agree on the same wrong answer.
The common gate's golden checks must be retained when sharing or batching work.

## Exact command duplicates

The following 33 command fixtures have the same source (apart from boundary
whitespace), expected exit and stdout as the mapped language fixtures, with the
same platform tags. All use plain `run`, no flags, mode `delete`, and no stderr
expectation. None appears in the extra command differential list; the mapped
language fixture already enters that list automatically.

They are **runtime duplication candidates**, not literally identical harness
contracts: command tests additionally check that the input survives and the
temporary executable is deleted. Preserve `002-run-deletes-executable.cmd`,
keep/cleanup/failure-sidecar cases and relevant integration tests. Language tests
also request HIR/LLVM sidecars, so retain a small plain-run CLI smoke test.
Empty command stdout means “unchecked”; language empty stdout means “must be
empty”, so the mapped language oracle is stronger for those cases.

| Command under `tests/commands` | Existing coverage under `tests/language` |
| --- | --- |
| [027-run-llvm-for-break-again.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/027-run-llvm-for-break-again.cmd) | [044-for-break-again.t](../tests/language/044-for-break-again.t) |
| [031-run-llvm-tuple-interpolation.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/031-run-llvm-tuple-interpolation.cmd) | [051-tuples.t](../tests/language/051-tuples.t) |
| [032-run-llvm-array-interpolation.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/032-run-llvm-array-interpolation.cmd) | [052-fixed-arrays.t](../tests/language/052-fixed-arrays.t) |
| [033-run-llvm-pointer-fields.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/033-run-llvm-pointer-fields.cmd) | [058-plex-ergonomics.t](../tests/language/058-plex-ergonomics.t) |
| [034-run-llvm-string-slices.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/034-run-llvm-string-slices.cmd) | [055-string-slices.t](../tests/language/055-string-slices.t) |
| [035-run-llvm-address-of-index.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/035-run-llvm-address-of-index.cmd) | [053-pointers.t](../tests/language/053-pointers.t) |
| [036-run-llvm-slices.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/036-run-llvm-slices.cmd) | [054-slices.t](../tests/language/054-slices.t) |
| [039-run-llvm-for-return.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/039-run-llvm-for-return.cmd) | [038-for-infinite.t](../tests/language/038-for-infinite.t) |
| [040-run-llvm-labelled-blocks.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/040-run-llvm-labelled-blocks.cmd) | [048-labelled-expression-block.t](../tests/language/048-labelled-expression-block.t) |
| [044-run-llvm-raw-unions.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/044-run-llvm-raw-unions.cmd) | [062-raw-unions.t](../tests/language/062-raw-unions.t) |
| [045-run-llvm-unit-enums.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/045-run-llvm-unit-enums.cmd) | [063-enum-unit-variants.t](../tests/language/063-enum-unit-variants.t) |
| [046-run-llvm-enum-payloads.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/046-run-llvm-enum-payloads.cmd) | [065-enum-payloads.t](../tests/language/065-enum-payloads.t) |
| [047-run-llvm-condition-on.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/047-run-llvm-condition-on.cmd) | [066-generalised-on.t](../tests/language/066-generalised-on.t) |
| [048-run-llvm-destructuring.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/048-run-llvm-destructuring.cmd) | [059-destructuring-bindings.t](../tests/language/059-destructuring-bindings.t) |
| [049-run-llvm-module-function-fields.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/049-run-llvm-module-function-fields.cmd) | [068-std-print-module.t](../tests/language/068-std-print-module.t) |
| [051-run-llvm-dynamic-slice-bounds.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/051-run-llvm-dynamic-slice-bounds.cmd) | [080-dynamic-slice-bounds.t](../tests/language/080-dynamic-slice-bounds.t) |
| [052-run-llvm-contextual-slice-literals.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/052-run-llvm-contextual-slice-literals.cmd) | [079-contextual-plex-and-slice.t](../tests/language/079-contextual-plex-and-slice.t) |
| [053-run-llvm-nested-array-literals.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/053-run-llvm-nested-array-literals.cmd) | [081-nested-array-literals.t](../tests/language/081-nested-array-literals.t) |
| [054-run-llvm-assignment-expressions.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/054-run-llvm-assignment-expressions.cmd) | [082-assignment-expressions.t](../tests/language/082-assignment-expressions.t) |
| [055-run-llvm-nil-pointers-and-slices.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/055-run-llvm-nil-pointers-and-slices.cmd) | [090-nil-pointers.t](../tests/language/090-nil-pointers.t) |
| [058-run-llvm-underscore-parameters.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/058-run-llvm-underscore-parameters.cmd) | [110-unused-underscore.t](../tests/language/110-unused-underscore.t) |
| [062-run-llvm-nested-on-branch.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/062-run-llvm-nested-on-branch.cmd) | [118-nested-on-branch.t](../tests/language/118-nested-on-branch.t) |
| [064-run-llvm-pointer-to-slice-cast.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/064-run-llvm-pointer-to-slice-cast.cmd) | [093-slice-casts-and-nil.t](../tests/language/093-slice-casts-and-nil.t) |
| [066-run-llvm-integer-pointer-casts.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/066-run-llvm-integer-pointer-casts.cmd) | [148-untyped-integer-pointer-casts.t](../tests/language/148-untyped-integer-pointer-casts.t) |
| [078-run-llvm-string-equality.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/078-run-llvm-string-equality.cmd) | [125-top-level-interpolated-strings.t](../tests/language/125-top-level-interpolated-strings.t) |
| [080-run-llvm-on-short-block-expr.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/080-run-llvm-on-short-block-expr.cmd) | [100-on-short-block-expr.t](../tests/language/100-on-short-block-expr.t) |
| [081-run-llvm-dynarray-typed-locals.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/081-run-llvm-dynarray-typed-locals.cmd) | [101-dynarray-typed-locals.t](../tests/language/101-dynarray-typed-locals.t) |
| [082-run-llvm-on-break-for-expression.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/082-run-llvm-on-break-for-expression.cmd) | [152-on-break-for-expression.t](../tests/language/152-on-break-for-expression.t) |
| [083-run-llvm-break-on.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/083-run-llvm-break-on.cmd) | [153-break-on.t](../tests/language/153-break-on.t) |
| [084-run-llvm-named-method-arguments.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/084-run-llvm-named-method-arguments.cmd) | [138-named-call-arguments.t](../tests/language/138-named-call-arguments.t) |
| [085-run-llvm-method-receivers.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/085-run-llvm-method-receivers.cmd) | [141-method-regressions.t](../tests/language/141-method-regressions.t) |
| [086-run-llvm-field-lvalues.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/086-run-llvm-field-lvalues.cmd) | [089-field-lvalues.t](../tests/language/089-field-lvalues.t) |
| [087-run-llvm-index-lvalues.cmd](https://github.com/Cthutu/nerd/blob/c861db2187534ced1d8c94e441f22a69d11f3490/tests/commands/087-run-llvm-index-lvalues.cmd) | [142-lvalue-and-plex-regressions.t](../tests/language/142-lvalue-and-plex-regressions.t) |

The first five mapped pairs were each run four times through their existing
harness (one warmup, three recorded passes). Median command times were
0.159, 0.262, 0.276, 0.264 and 0.265 seconds; language counterparts were
0.160, 0.278, 0.277, 0.279 and 0.278 seconds. All passed.

**Estimated saving:** extrapolating the command sample mean (0.245 seconds) to
33 removals gives approximately **8.1 seconds per fixture pass**, or 16.2 seconds
across debug/release passes if costs were equal. Only five pairs were timed;
release-compiler performance and the remaining 28 pairs were not measured.
This removes 33 repeated compiler/toolchain launches, not 33 language behaviours.
It does not reduce the differential matrix.

## Feature batching: the `on` proof

The opt-in experiment takes the existing sources from commands
`062-run-llvm-nested-on-branch`, `129-run-on-expression-return-branches` and
`252-run-nested-partial-on-statement`. It renames each `main` into a helper and
calls it from one new `main`, retaining the original return-code check and
stdout. The transcript brackets each helper:

```text
begin 062-run-llvm-nested-on-branch
hit 2
pass 062-run-llvm-nested-on-branch
begin 129-run-on-expression-return-branches
120
pass 129-run-on-expression-return-branches
begin 252-run-nested-partial-on-statement
pass 252-run-nested-partial-on-statement
completed 3 cases
```

The checked-in evidence includes the complete generated Nerd source. An omitted
middle case still returns success and prints `completed 3 cases`, but fails exact
transcript comparison. The completion line alone is not a sufficient oracle.
The proof verifies stderr is empty as well as exit and stdout; no snapshots were
removed, and the batch is not registered in any production suite.

Separate LLVM build+run medians totalled 0.643 seconds versus 0.245 seconds batched:
**0.398 seconds / 62% saved in this sample**, reducing three builds to one.
The command duplicate measurement and this sample overlap (`062` is a duplicate);
their gains cannot simply be added. Moving a unique command fixture into language
also adds automatic C differential coverage, which can initially increase cost.

One exploratory candidate, `080-run-llvm-on-short-block-expr.cmd`, emits a debug
heap-leak diagnostic while returning success. Its existing harness does not
assert empty stderr; the experiment rejected it under its stricter oracle. It
was excluded, not silently modified or “fixed” during the audit. Resource cleanup
and expected diagnostic behaviour need review before batching such cases.

### Proposed organisation and rules

- Start with an `on` feature family: boolean/value/range/string/payload patterns,
  guards, binder mutation, partial statements, expression results and control
  flow. Keep a documented manifest of original regression IDs and assertions.
- Use a helper per scenario, unique labels and exact expected transcript. A
  hundred scenarios can share a build; split when compile-time environments,
  lifetimes or debugging practicality require it, not after an arbitrary count.
- Keep a manifest-driven selection mechanism for rerunning one scenario without
  recompiling every separate fixture. Stable scenario names retain regression
  searchability and failure attribution. Assertions should include case context;
  print start/pass markers and ensure failure output is flushed.
- Assert intermediate values, side-effect counts and a final completion marker.
  An empty successful program must never satisfy the expected transcript.
- Keep top-level conditional compilation under separate define/platform
  configurations: one executable cannot exercise both presence and absence of a
  declaration. Runtime `on` and build/source `on` need different configurations.
- Separate expected process termination, panic/abort, stdin/arguments, `main`
  signatures, global initialisation, external module layouts and ABI hosts where
  aggregation changes what is tested. Keep subprocess isolation for these.
- Explicitly reset shared state and release resources between cases. Separate
  executables currently provide this isolation for free. Avoid accidental name
  resolution, interprocedural optimisation or ordering dependencies.
- Preserve small targeted HIR/LLVM checks for properties invisible in output
  (metadata, symbol linkage, ABI layout, lowering shape). A monolithic snapshot
  for an entire feature would be fragile; runtime batching need not imply that.

## False confidence to fix during migration

Seventeen `check` command fixtures expect a nonzero exit but assert neither
stdout nor stderr (full list in the JSON inventory). The corresponding list of
all nonzero-exit commands also includes intentionally nonzero successful programs;
those must not automatically be classified as diagnostic tests.

The negative checks can pass for an unrelated parser/import/type error. Add
structured expected diagnostics, preferably as isolated source/JSON pairs in
`tests/errors`, while keeping a small `nerd check` CLI contract regression.
Moving several such pairs into one `.e` file improves organisation but **does not
reduce compilation count**: the existing runner launches one compiler per pair.
Batching invalid sources in one module could mask later errors through recovery
or an earlier failure and should not be the default optimisation.

Concrete example: `350-check-atomic-literal-range.cmd` uses
`0x1_0000_0000_0000_0000` assigned to `atomic[u8]`. The observed diagnostic is
**“Integer literal is too large”**: it tests the literal parser's range, not
whether 256 is rejected for an eight-bit element. The known plain/atomic element
range bug is therefore still uncovered by this passing case. Give parser overflow
and destination-range checking distinct names and expectations once that bug is
fixed. Checked callback-return and typed-atomic mismatch cases currently produce
the intended diagnostics, but their fixtures do not lock those diagnostics down.

## Ranked migration milestones

| Order | Work | Exit criteria / expected benefit |
| --- | --- | --- |
| 1 | Establish per-suite/per-case wall times and process counts in normal runs | Windows, Linux, WSL baselines; record compiler configuration, tool versions, skipped cases; no invented whole-suite speedup |
| 2 | Remove the 33 mapped runtime duplicates after preserving CLI cleanup contracts | Remaining language and cleanup cases pass debug/release; exact mapping retained; ~8 s per fixture pass estimated on this Windows host |
| 3 | Migrate an `on` batch with labelled scenarios and golden output; strengthen migrated negative diagnostics | Deliberately omitted/wrong-result case fails; LLVM/C pass; IR checks and configuration-sensitive tests retained; expand feature-by-feature |
| 4 | Share same-run LLVM golden/differential baselines | Avoid repeat compiles without losing stderr/stdin/path/IR checks; keep standalone runners working; measure actual wall time |
| 5 | Deduplicate repeated native layout builds and network C generation | Layout smoke once per host/toolchain per gate; network currently calls C generation again for `--copts`, whereas thread runner combines them; preserve artifact and option assertions |
| 6 | Consider HIR/LLVM emit-only modes and bounded fixture parallelism | Keep verification needed by each test; validate compiler independence/artifact paths and resource limits before concurrency |

The common gates currently repeat socket layout compilation four times under
`just test` and two under `test-release`; thread layout twice per gate. These
independent C layout facts do not depend on which Nerd backend generates a
program. Share those checks within an orchestrated gate, retaining standalone
runner validation. Savings are unmeasured and likely smaller than batching.

Do not first weaken `just test` into a smoke suite or remove release checks.
Consolidation and shared work can reduce cost while preserving its contract.
A separate explicit quick-development recipe may be useful later, but should
not be described as equivalent to the full gate.

## Verification of this audit

- Generated the inventory from the real fixture collector and C differential list.
- Ran ten original fixtures (five mapped pairs), each four times, all passing.
- Ran separate and combined `on` sources at LLVM, then verified combined C O0/O2.
- Verified a successful program with an omitted scenario fails the golden output.
- Inspected actual diagnostics for atomic FFI, literal overflow, typed atomic and
  callback signature errors. No compiler bug was changed in this branch.
- Existing suite/fixture files and default Justfile recipes are unchanged.

The full `just test`, `just test-release`, Linux and WSL gates were not rerun for
this audit-only branch. No production test was removed; the next branch can make
the first small consolidation with this mapping as its review checklist.
