# Compiler construct repairs — Linux validation, 20 October 2026

Tested on `std-library`, based on `2dbd6ef5`, with an initially clean worktree.
[sources.sha256](sources.sha256) records the changed compiler, documentation and
regression sources. Both requested language extensions were approved before
implementation. Compiler concurrency defaults are unchanged.

## Fixes and regression coverage

| Request | Result | Language regression |
| --- | --- | --- |
| 1. Repeated Default construction | Initializers are emitted only through their declaration; nested scopes, loops and owning fields execute once. | `244-default-once.t` |
| 2. Plex destructuring | LLVM and C support mutable, immutable, typed, assignment, renamed and nested patterns; source evaluation happens once. Selected boxes move and overwritten boxes are released. A dedicated HIR count prevents recursive block traversal. | `245-plex-destructuring.t` |
| 3. Nested generic applications | Recursive argument collection preserves outer and inner type lists, including tuple types, aliases, fields, parameters, returns and qualified applications. | `246-nested-type-applications.t` |
| 4. Explicit function type arguments | Comma lists work on functions, methods and qualified functions with nested arguments; both backends preserve imported specializations. Value indexing remains separate. | `247-explicit-type-arguments.t` |
| 5. Applied-type associated calls | Applied local and qualified generic types resolve before runtime member lookup, including multiple and nested arguments. | `248-applied-associated-calls.t` |
| 6. Tuple mutation | Direct and compound writes follow root mutability, including nested projections, pointer parameters and tuple fields in records. | `249-tuple-mutation.t` |
| 7. Builtin Display | Canonical Display constraints and callable `.show()` share the builtin interpolation boundary; user implementations retain normal lookup. | `250-builtin-display-constraint.t` |
| A. Condition truthiness | Logical operators, chains and guards accept optional/result presence, short-circuit, and treat present `no` as present. Bool assignments/parameters remain explicit. | `251-condition-truthiness.t` |
| B. Scientific notation | Decimal exponents support signs, separators, contextual f32/f64 typing and long mantissas without integer overflow. Hex integers retain their meaning. | `252-scientific-floats.t` |

The language fixtures are in `tests/language/`. New error fixtures 152 and 153
cover malformed exponents, immutable tuple storage, generic arity, malformed
argument lists, value indexing, missing associated members, duplicate field
selection, unsupported Display and explicit bool-conversion boundaries. Existing
condition diagnostics were updated to state the accepted condition types.
Formatter fixture 224 checks direct type arguments, nested applications, exponent
spelling and tuple projection chains. HIR expectations remove standalone default
expressions that previously caused duplicate evaluation.

Learner documentation is updated in manual parts 03 and 08. Specification changes
cover lexing, expressions, statements and the runtime model. The implementation
map is in `docs/overviews/INTERNALS.md`, under “Compiler construct repairs”.

## Verification

- `just test`: **1,220 passed, zero failed, nine existing Linux skips**, plus all
  applicable auxiliary gates. This includes **316 C differential fixtures at
  O0/O2**, with zero platform skips. See [just-test.log](just-test.log).
- `just test-release`: **1,220 passed, zero failed, nine existing Linux skips**,
  plus all applicable release auxiliary gates. See
  [just-test-release.log](just-test-release.log).
- `python3 build/test_render_threads.py --sanitize address`: full frontend
  scheduling, output identity, diagnostics and cleanup checks passed. See
  [frontend-asan.log](frontend-asan.log).
- `python3 validation/linux/results/20261020-compiler-constructs/native-probe.py`:
  all nine new language fixtures passed through LLVM and generated C in debug
  and release modes (**36 executions**). Generated C also runs with ASan, UBSan
  and leak detection. See [native.log](native.log).
- `python3 validation/linux/results/20261020-compiler-constructs/compiler-asan-probe.py`:
  an ASan compiler compiled all nine positive fixtures, rejected all 19 new
  negative cases with located diagnostics, and compiled a 24-level nested
  generic program that returned 42. See
  [constructs-compiler-asan.log](constructs-compiler-asan.log).

Run the probes from the repository root. They use temporary directories and
require the debug compiler, Clang and the repository's normal build dependencies.

## Remaining boundaries

Destructuring follows existing explicit aggregate-field cleanup rules; this
change does not add recursive aggregate destruction. Ignored fields of an
existing aggregate retain ownership. Tests explicitly release remaining owning
fields. Do not infer automatic deep cleanup for discarded temporary aggregates.

Automatic aggregate Display applies when all recursively contained elements are
builtin displayable; it does not synthesize aggregate implementations from custom
element `.show()` methods. Scientific floats retain the existing f64 `strtod`
range policy, including rejection on ERANGE. Native Windows execution has not
been validated by this Linux run.
