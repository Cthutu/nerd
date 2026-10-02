# First test suite consolidation

Branch: `test-suite-consolidation`, based on `std-library` at `6bc00639`.
The [historical audit](test-suite-audit.md) was merged as `007f76a2` without
rewriting either branch. This branch implements its first consolidation steps;
it does not change the default gate's scope or weaken release/backend coverage.

## Changes

- Removed **33 exact runtime duplicates** from `tests/commands`, after rechecking
  source, exit, stdout, platforms and invocation flags against current language
  fixtures. Each language replacement retains its existing HIR/LLVM assertions.
- Replaced **14 additional `on` runtime command fixtures** with
  [one language fixture](../tests/language/200-on-regressions.t). Each scenario has
  its original regression ID, helper function, original return/output checks,
  and exact begin/pass transcript. The final completion marker cannot by itself
  make a skipped scenario pass.
- Removed the corresponding 12 command entries from the C differential list.
  Language discovery automatically includes the new batch, so those behaviours
  retain LLVM/C coverage; two formerly LLVM-only scenarios gain C coverage.
- Preserved dedicated command source-survival/deletion/keep/failure-cleanup
  tests, all other CLI/artifact/platform/module tests, and **all existing** HIR,
  LLVM, format, LSP and structured-error fixtures.

The [machine-readable mapping](test-suite-consolidation-map.json) records every
removed path, its replacement/scenario, original output/exit, type renaming and
previous differential membership. Historical sources remain available at
`007f76a2`; the verification script retrieves them from Git without restoring
them into the active suite. Types `Payload`/`Thing` were given scenario suffixes
to avoid collisions; helper bodies and historical control-flow forms are retained.
Command 255 formerly passed `--llvm`; the language runner already passes that
flag as well as `--hir`. No command-specific IR assertion existed to drop.

All migrated cases are successful runtime programmes, including intentional
nonzero `main` results now checked at helper return. No negative diagnostics were
merged or removed. The 23 structured literal destination-range cases added on
`std-library` remain intact, as do the updated atomic and integer-wrapping tests.
The audit's literal-range bug is now fixed by the inherited `6bc00639` baseline;
its old lexer-overflow observation remains historical evidence.

## Compiler bug exposed by batching

The combined file initially rejected a valid result initializer that passed in
isolation. A preceding enum with a structurally equivalent plex payload caused
`sema_materialise_type` to intern another enum entry and lose `STF_Optional` or
`STF_Result`. The minimal trigger is:

```nerd
First :: plex { width u32 height u32 }
Envelope :: enum { Done(First) Other }
Second :: plex { width u32 height u32 }
main :: fn () {
    value: Second\string = { width: 1 height: 2 }
    _ := value
}
```

Commit **`58818307`** preserves those flags on the canonical type. Scenario 233
also checks the optional form following the equivalent earlier enum payload.
The batch passed fresh debug/release compilers and LLVM/C O0/O2 before removal
of any old fixture. [Compiler internals](overviews/INTERNALS.md) describes the fix.
This interaction was fixed rather than avoiding the failing combination.

## Counts and measurements

| Category | Before (`6bc00639`) | After | Change |
| --- | ---: | ---: | ---: |
| Commands | 359 | 312 | -47 |
| Language files | 204 | 205 | +1; 14 labelled scenarios |
| All fixture entries | 1,208 | 1,162 | -46 |
| Active Windows fixture entries | 1,193 | 1,147 | -46; same 15 platform skips |
| Error files / isolated diagnostic pairs | 127 / 409 | 127 / 409 | unchanged |
| HIR / LLVM fixtures | 28 / 48 | 28 / 48 | unchanged |
| Active Windows differential fixtures | 300 | 289 | -11; same two platform skips |

Counts are discovered statically; final gate results are recorded in the linked
Windows evidence. Logical behaviours were retained rather than deleted to achieve
the lower fixture counts. The C matrix now has 205 language files, 79 selected
commands and seven C-only files, including platform skips.

The changed fixture workload was timed using **the same freshly built fixed debug
compiler and checkout modules** on both sides. The old 47 command fixtures were
extracted from Git to a temporary directory and run through `test_command`; the
new batch ran through `test_language`, including its HIR/LLVM flags. The unchanged
33 alternative language fixtures are outside both timed sides.

After one excluded warmup, three sequential runs gave:

| Changed work only | Median wall time |
| --- | ---: |
| Old 47 command fixtures | 10.273 seconds |
| New 14-scenario batch | 0.317 seconds |
| Removed repeated work | **9.956 seconds per debug fixture pass** |

This is **not a 97% full-gate speedup**. It measures only the replaced work, on one
Windows host, including compiler/toolchain execution and harness cleanup. It does
not include compiler builds, unchanged tests, release-compiler performance,
auxiliary gates, cold-machine behaviour or C differential savings. Those require
separate measurements. The compiler SHA-256 and all samples are recorded in
[timing evidence](../validation/windows/results/20261002-test-suite-consolidation/changed-work-timings.json).

## Reproduce and verify

```sh
python build/check_test_consolidation.py --compiler _bin/nerd-debug.exe --output verification.json
python build/check_test_consolidation.py --compiler _bin/nerd-debug.exe --measure --output timings.json
python build/test.py --nerd _bin/nerd-debug.exe --filter 200-on-regressions
just test
just test-release
```

The [verification script](../build/check_test_consolidation.py) checks the exact
duplicate mappings, retained CLI fixture contents, all original command behaviours,
batch golden output and differential membership. It compiles two deliberate
mutations: omitting a scenario while returning success must fail stdout comparison;
retaining the labels but returning a wrong value must fail the batch's assertion.
Both negative checks passed with the debug compiler. This is an opt-in migration
check, not another default suite of repeated cases.

The original audit proof remains standalone: `build/audit_on_batch.py` reads its
three historical fixtures from the pinned Git revision, even after their removal.
No production harness was redesigned and no new Python gate was added to Justfile.

## Validation and remaining milestones

Focused debug/release and C O0/O2 batch checks, all 47 old command behaviours,
exact mappings and omission/wrong-result checks pass. Complete native debug and
release common gates are being run against the committed consolidation. Final
counts, logs and scope are recorded in
[Windows evidence](../validation/windows/results/20261002-test-suite-consolidation/README.md).

Remaining work: consolidate more compatible feature scenarios, add structured
diagnostic expectations to negative `check` cases, share same-run LLVM golden
and C-differential reference builds, remove repeated layout/C generation work,
and profile emit-only/parallel-runner opportunities. Platform defines, process
termination, global initialization, ABI hosts and snapshot-specific tests need
individual review before batching. Linux/WSL gates remain for the receiving PC.
