# Compiler issues found during feature-test cleanup

## Fixed: interpolation ignored a pointer-receiver Display implementation

A `World` plex with `show(self: ^World)` compiled but interpolated structurally.
The previous core requirement was by value; imported trait signature checks
incorrectly used foreign node indices in the local AST and could skip validation.
Backend Display discovery then required an exact by-value receiver match.

Core Display now requires `show(self: ^Self) -> string`. Imported requirements
are checked in their defining module, and mismatched receivers/returns are
diagnosed. Sema selects interpolation methods and HIR emits normal borrowed
calls, with explicit implementations taking precedence over built-in formatting.
`display-borrow.t` checks the World reproduction, receiver identity, exactly-once
evaluation, noncopyable storage and generic dispatch; `display-signatures.e`
checks invalid canonical and imported trait signatures. Both backend paths and
the existing imported-display and result-main tests exercise the new contract.

The teaching reference is `/home/matt/nd`; it was not changed. Use valid teaching
examples and intended language rules as regression contracts. A transformed
batch failing is not sufficient evidence of a compiler bug. The failed batching
trials included changed names, missing returns and contextual-type changes;
those were fixed by preserving independent source programs, not by changing the
compiler or weakening the language contract.

## Fixed: C does not consume a global box source

The unchanged historical `157-run-box-move-and-cleanup` program returned 42 with
LLVM but 10 with C: after passing `global_map` to a box parameter, C left the
source non-nil. The failure existed before this cleanup and was exposed when
moving the test into language discovery expanded C coverage.

Ownership requires assignment, box parameters and returns to transfer the owner
and clear its source, as taught in chapter 18 and specified in the runtime model.
`cgen_consume` handled local slots only. It now also clears mutable global value
bindings, matching LLVM. The original program and a smaller
`global-box-transfer` scenario in `tests/language/boxes.t` check calls, assignment
and returning a global owner. C differential checks retain both programs.

## Fixed: C cannot compile a three-part string continuation

Minimal valid reproduction, using continuation syntax from the teaching guide:

```nerd
main :: fn () {
    prn("one "
        +"two "
        +"three")
}
```

LLVM printed `one two three`; generated C failed to compile with
`cannot take the address of an rvalue`, at `nrt_string_builder_finish(&0, ...)`.
The inner concat node lacks an inferred type. C emission now selects the builtin
string type rather than treating it as void, matching literal/LLVM handling.
`continued-literal-three-parts` in `tests/language/strings.t` checks three/four
parts and a constant binding. The unchanged PNG/JPEG decode probes also compile
and pass C after this fix. They are not excluded from differential execution.

## Teaching verification and investigation boundaries

The existing `_verify/run_chapters.py` defines 37 run/source-test outcomes and
three object/library/shared-library builds. All **40 checks passed** against
copied, unchanged sources with the current compiler. Inputs and auxiliary assets
were copied to Nerd's ignored `_tmp/cleanup/teaching`; the original scripts and
teaching checkout were not rewritten. The 16 additional plex-destructuring variants, valid issue probes and root
examples also passed through both LLVM and C with their original source intact.
Both C fixes passed ASan, UBSan and leak detection in debug/release target modes.
No valid example in these checks failed the current compiler.

The documentation's `_verify/issues/exponent.n` calls `prn(value)` with an f64;
that call is invalid under the documented string printing API. The corrected
`exponent-current.n` uses interpolation and passes. That older probe's print
error is not evidence that scientific notation regressed. Its original file was
left intact. Likewise the existing probes for default invocation count, nested
applications, explicit arguments, associated calls, Display, tuple mutation and
truthiness exercise previously requested compiler repairs; they are independent
from failures caused by mechanically renaming snippets.

Some prose still describes limitations where these approved features now work.
[The feature inventory](test-features.md#teaching-coverage-and-contract-discrepancies)
records the precise differences without changing the teaching reference or
rolling back approved work. Internal manuals/specs are checked separately and
updated where they omitted established behaviour. Native Windows execution of
the reorganised suite and the two C fixes remains outstanding.

## Fixed: qualified imports invalidate alias-classification pointers

Resolving a qualified generic alias, such as `IntegerCell :: cells.Cell[i32]`,
can append imported declarations. Alias classification retained a pointer into
that resizable declaration table and wrote through it after resolution.
AddressSanitizer identified a heap use-after-free. Classification now reacquires
the declaration by its stable index after recursively resolving the RHS.
The qualified-generic program in [generics.t](../tests/language/generics.t)
retains aliases, nested instantiations, wide values, local module aliases,
associated constructors and explicit/inferred function calls as one regression.
