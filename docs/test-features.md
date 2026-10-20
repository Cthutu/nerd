# Test features and teaching coverage

The taxonomy follows [cthutu/nerd-docs](https://github.com/Cthutu/nerd-docs/tree/b7204cf1cab979bb5abcf31367ab14552d5ec259/docs/learn),
reviewed from `/home/matt/nd` at `b7204cf1cab979bb5abcf31367ab14552d5ec259`.
That checkout matches its fetched `origin/main`; it was read, not edited.
Chapter numbers below refer to that revision. The teaching examples are the
language reference for this review; compiler failures against valid examples
must be reproduced, tracked and fixed. Current implementation behaviour alone
does not establish the intended contract. The teaching checkout was not edited.

## Feature taxonomy

Use a feature stem across `language`, `sema`, `errors` and `format` where applicable.
Not every feature needs a file in every family. Keep related edge cases in the
same feature, rather than making a new numbered regression file.

| Feature stem | Teaching chapter | Scope |
| --- | --- | --- |
| `bindings` | 2, 4 | Constants, variables, scope, inference, assignment and definite assignment |
| `primitives` | 3 | Numeric literals, scientific notation, operators, casts and size |
| `records`, `enums`, `tuples` | 5 | Plex/union layout and fields, enum payloads, tuple projections and destructuring |
| `arrays` | 6 | Fixed/runtime arrays, slices and dynamic-array operations |
| `pointers` | 7 | Addressing, dereferencing, arithmetic and pointer compatibility |
| `arenas` | 8 | Allocation, growth, marks, cleanup and temporary arenas |
| `branching` | 9 | `on`, patterns, guards, chains and condition truthiness |
| `error-handling` | 10 | Optionals, results, fallback, propagation, defer and undo |
| `loops` | 11 | Condition/C-style/iterator loops, labels, ranges and loop values |
| `generics` | 12 | Applications, inference, explicit arguments and specialization |
| `methods`, `traits` | 13 | Receivers, associated functions, constraints and canonical traits |
| `strings` | 3, 14 | String literals, interpolation, indexing and conversion |
| `modules` | 15 | Imports, visibility, reexports, folder modules |
| `functions` | 16 | Function values, recursion, compound functions, named/default arguments |
| `compile-time` | 17, 20 | Defines, compile-time parameters, locations, embedding and pragmas |
| `boxes` | 18 | Allocation, ownership moves, borrowing and explicit release |
| `assertions`, `source-tests` | 19 | Assertion effects and Nerd source-test syntax/diagnostics |
| `atomics` | 21 | Atomic values, operators, methods, orders and inference |
| `ffi` | 16, 22 | C functions, callbacks, variadics and ABI compatibility |
| `build-settings` | 23 | Source build configuration and guards |
| `syntax` | Appendix A | Lexical boundaries and malformed syntax without a more specific feature |
| `library-apis` | Beyond the language chapters | Library API smoke checks; bounded library runners retain their own contracts |
| `formatter-layout` | Editor/layout contract | Comments, whitespace, recovery, wrapping and alignment |

Errors are grouped by the construct at fault. Mixed historical files were split:
for example immutable tuple fields go in `tuples.e`, generic arity/member lookup
in `generics.e`, and explicit bool-conversion boundaries in `error-handling.e`.
A regression involving several features belongs under its primary contract;
its source can still exercise other features without duplicating the case.

LSP files follow editor operations instead of chapters: completion, hover,
signature help, definition, references, rename, code actions, document links,
document/workspace symbols, semantic tokens, and diagnostics/synchronization.
Each source/requests/expected triple is an independent session. Formatter files
use chapter features plus layout/recovery. Distinct ranges, diagnostics, edits,
request sequences and source spellings remain separate assertions; grouping
alone is not evidence that two cases are redundant. Exact-pair/triple review
found no identical formatter/LSP cases safe to delete.

## Runtime and snapshot review

Ordinary scenarios use one larger feature program where namespaces can coexist.
Each scenario prints a begin label, its original output and its checked result.
Missing calls therefore fail stdout comparison. Helper return values use the
same low-byte exit oracle as the historical harness. CLI entry-point width and
argument contracts remain independent process tests.

Global names, canonical traits and structural types can interact across snippets.
The failed mechanical trials are transformation failures, not evidence of new
compiler bugs. Generic, trait, method, module and FFI programs were restored to
unchanged declarations and `main` functions inside named independent blocks.
Other location, stdin, platform and ownership-sensitive programs use the same
isolation. They share feature files but are compiled separately; fewer fixture
files does not imply a single compiler invocation per feature.

All removed language/command paths were accounted for against the local trial
mappings. Existing isolated programs retain their HIR/LLVM oracles. The ordinary
batches omit repeated full runtime declarations and debug metadata; dedicated
HIR/LLVM suites remain. Additional `arrays`, `branching` and `records` snapshots
preserve original source and lowering assertions from the removed dynamic-array,
untyped-`on`, and aggregate-lvalue regressions. LLVM expectations retain generated
function bodies rather than repeated external-runtime declarations. The dedicated
optional/result lowering suite continues to cover wrapper representation.

Ordinary runtime cases were moved out of `.cmd`, including `-r` regressions and
cases that merely requested `--llvm`. Release metadata remains active in both
LLVM and C runs. Commands retained for default source discovery, executable
cleanup, host/object/shared-library artifacts, arguments, abort/exit/shutdown,
allocator diagnostics and error rendering still check those process contracts.
Two old dynamic-array content probes and the global enum stress probe now free
their allocations explicitly: incidental shutdown leak sizes were not their
oracle and differ between backend representations.

## Backend boundaries

The C differential gate discovers every independent language program and runs
native C at O0/O2 against LLVM. All previously selected differential contracts
remain covered after migration. Broader coverage exposed two existing C bugs:
global box moves did not clear their source, and nested continuation literals
could generate an address of `0` instead of string storage. Both now have small
regressions and compiler fixes. PNG/JPEG decoding also passes after the string
fix; it was a downstream symptom, not a third independent bug.

There are no C exclusions for these cases. The [compiler issue record](compiler-issues.md)
contains reproductions, fixes and teaching-example verification. Literal embedded
assets resolve against the fixture before C-test relocation.

## Teaching coverage and contract discrepancies

These are specific differences at the pinned teaching revision, rather than
claims that whole chapters are missing or that the teaching examples fail.
The previously approved compiler extensions remain covered. References to
implementation limitations are recorded separately from intended language
rules; no changes were made to `nerd-docs`. See the issue record for direct
execution of its existing verification programs.

| Category | Teaching location | Verified coverage / discrepancy to track |
| --- | --- | --- |
| Limitation note differs | Ch. 3, numeric literals | Scientific forms `1e3`, `1e-3`, `1E+3` work; `primitives.t` checks contextual floats and malformed exponents are in `primitives.e`. The positive exponent probe passes. |
| Limitation note differs | Ch. 5, tuple copying | Fields rooted in mutable tuple storage accept direct/compound assignment; `tuples.t` covers nested and pointer projections, `tuples.e` rejects immutable roots. |
| Limitation note differs | Ch. 5, plex destructuring | Runtime lowering supports named, renamed and nested patterns; `records.t` covers single evaluation and box ownership transfer. The existing plex-destructuring verification programs also run successfully. |
| Workaround note differs | Ch. 12, nested type applications | `Cell[Cell[i32]]` works directly, including qualified/nested positions; `generics.t` retains the original nested-application regression. An inner alias is optional. |
| Limitation note differs | Ch. 12, several type parameters | Multiple explicit function arguments, including methods and qualified functions, parse; `generics.t` checks them. Malformed-list diagnostics remain checked. |
| Workaround note differs | Ch. 13, generic implementation blocks | `Cell[i32].init(...)` and qualified applied receivers work; `generics.t` preserves the associated-call regression and both returned fields. |
| Limitation note differs | Ch. 13, trait constraints | Canonical `Display` accepts built-in formatting types and supplies callable `.show()`. `traits.t` covers primitive and recursively built-in aggregate formatting. Custom-element aggregate synthesis remains outside this bridge. |
| Condition boundary differs | Ch. 10, truthiness | Optionals/results work in `&&`/`||`, chains and guards; a present `no` tests presence. `branching.t` covers short-circuit effects; boolean assignments/parameters remain explicit and have negative tests. |
| Missing convenience example | Ch. 18, several elements | Implicit `box[T]` to `[]T` borrowing is supported, in addition to the documented pointer conversion and explicit `.data` cast. `boxes.t` covers inference, nil/empty boxes, mutation, optional wrapping and owner lifetime boundaries. |
| Advanced example gap | Ch. 12–13 | Erased callback specialization and inference through generic task/receiver shapes have tests in `generics.t`; the general generic-method chapter does not show these API patterns. Add focused examples rather than exposing internal specialization symbols. |
| Library/API material outside chapter scope | No library API chapters | Raptor, Nexus, network, term/gfx/io smoke and lifecycle tests are API coverage, not missing language syntax. Keep their tutorial/reference backlog distinct from language gaps. |
| Intentionally implementation/editor focused | No teaching requirement | HIR/LLVM identities, C parity, source locations, diagnostic JSON, completion edits and process/artifact cleanup need engineering docs and regressions, not a learner chapter for each assertion. |

The separate teaching checkout was not modified. Its broad coverage already
includes bit fields/layout, runtime arrays, compound functions, default/named
parameters, macros, compile-time parameters, source tests, atomics and advanced
FFI; these are not classified as missing merely because their tests were formerly
scattered across command fixtures.

## Repository footprint and evidence

The cleanup is based on `fac0feae`. Historical measurements and validation outputs
are available through the immutable [measurement archive](https://github.com/Cthutu/nerd/tree/fac0feae825d73a478466410956c1ca7749a2a8a/review/measurements)
and [Windows archive](https://github.com/Cthutu/nerd/tree/fac0feae825d73a478466410956c1ca7749a2a8a/validation/windows/results).
Current Windows handoff instructions live in
[validation/windows/HANDOFF.md](../validation/windows/HANDOFF.md).
Generated evidence stays ignored; keep maintained commands and conclusions,
not copied raw logs. Removing files reduces checkout/current-tree payload; it
does not shrink existing Git history or rewrite remote objects.

The review reduced language fixture files from 227 to 27, error files from 139
to 26, formatter files from 224 to 25, and LSP files from 218 to 14. All 498 original error
cases, 224 formatter pairs and 218 LSP sessions remain (514 error cases after
promoting 16 negative command checks). Commands fell from 318
to 116; all 429 removed language/command fixtures are accounted for in feature
files. Eleven additional positive semantic checks are grouped without linking,
and 16 ordinary negative command checks now have structured diagnostic oracles. Twelve shared runtime programs contain 192 labelled scenarios; other
programs preserve their original independent namespaces. File counts are not
comparable to scenario counts or to historical runner pass totals.

Tracked current-tree payload falls from approximately 85.9 MiB to 9.2 MiB
(about 77 MiB removed), chiefly by deleting historical raw outputs. The old 83
selected C command contracts were accounted for in retained commands or language
discovery. Dedicated HIR/LLVM coverage also gains three focused fixtures in each
family. Verification commands and final results belong in the completed change
summary; raw logs remain under ignored `_tmp/cleanup`.

## Cleanup verification (20 October 2026)

- Full debug and release gates: 309 grouped fixture files passed, zero failures,
  nine Linux platform skips, plus their applicable auxiliary checks.
- C differential verification under both compiler builds: 257 programs in 46
  files at O0/O2; one Windows-only file skipped, no backend exclusions.
- Five grouped-harness tests and six Windows-runner plan tests passed. The latter
  are portable harness checks, not native Windows compiler validation.
- The existing 40 teaching verification checks and 16 additional programs passed;
  the latter also passed C. The teaching checkout was left unchanged.
- Both C regressions passed ASan/UBSan/leak checks for debug/release targets.
- Formatting, installation and installed compiler/editor smoke checks passed.
- Final taxonomy moves were checked again with the error and formatter families;
  all 498 original diagnostic source/oracles, 224 formatter pairs and 218 LSP
  sessions were accounted for. New negative checks bring diagnostics to 514.

Native Windows execution remains pending in the maintained handoff. Optional
network/Nexus integrations were removed from default gates; recipe forwarding
was checked explicitly, without claiming a new integration stress run.
