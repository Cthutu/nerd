# Internals

On Windows the direct LLVM linker includes `legacy_stdio_definitions.lib` with
the SDK/CRT libraries. Nerd FFI declarations call symbols such as `vprintf`
directly, so they need the out-of-line definitions normally supplied through
the C headers. No C compiler driver is involved in native Nerd linking.

The generated host entry wrapper calls Nerd `main` with its actual semantic
integer return width, then sign-extends, zero-extends or truncates to the host
32-bit process status. Calling an i8/i16 main as i32 leaves upper return-register
bits undefined on Windows; POSIX's low-byte exit status can hide this bug.
Both console and windowed entry wrappers use the same conversion, including
entry points accepting command-line arguments.

Windows direct LLVM debug builds embed DWARF in the executable via
`lld-link /debug:dwarf`. Installation checks inspect source line tables with
`llvm-dwarfdump`; CodeLLDB stepping checks validate their usability.
C output names replace the requested output root's extension directly, before
any Windows executable suffix is appended, so `--cgen -o program.c` stays
`program.c` rather than becoming `program.c.c`.

C output creation on Windows retries only sharing and lock violations for at
most 50 ten-millisecond waits. Repeated C differential/front-end runs can race
with external readers of the preceding output. Other open errors fail
immediately; persistent locks still diagnose failure and preserve the old
file. The C CLI regression holds a native read handle to test both temporary
and persistent sharing violations without relying on scanner timing.

This document is the high-level map of the current codebase. It points at the
main subsystems and tells you where to read next.

For unknown-name diagnostics, Sema examines the current module's deduplicated
direct imports and their already-loaded semantic declarations. Matching private
declarations contribute per-module `pub` suggestions through the error helpers;
type diagnostics filter out value declarations. Diagnostic discovery neither
loads new modules nor follows transitive imports or re-exported declarations.

Optional `?T` and result `T\E` types are semantic built-ins. Sema interns them
as flagged tagged-sum types with private variants; HIR contextualises
presence/success/error construction and has a dedicated propagation node. The
LLVM backend uses the ordinary enum tag-plus-payload layout and emits the early
return path for propagation, including deferred cleanup.
Sema also diagnoses propagation at the postfix `?`: when the enclosing function
has no matching optional or result channel, the diagnostic names the propagated
failure type and suggests a compatible enclosing return type.
Recursive semantic type rendering isolates nested names in temporary arenas
before appending them, keeping composite diagnostics such as
`^PixelLayer\GfxError` contiguous and source-accurate.
Parser recovery and semantic index checking recognise the extraction-shaped
`on value[payload]` mistake, including an empty branch body, so diagnostics can
point to the binder and suggest the required `=> [payload]` syntax instead of
blaming the closing brace or reporting that a result is not indexable.

The parser provisionally creates implicit binders for bare-identifier `on`
subjects before their types are known. Sema retains these only for branches
that extract a payload. Boolean branches and optional absent branches resolve
references back to the original subject, preserving assignments and mutability
checks. Otherwise, updates such as `on ready => ... else { ready = yes }`
would write a branch-local copy and leave the surrounding state unchanged.
The C backend converts untyped `nil` to an absent optional before considering
payload promotion, including when evaluating omitted default arguments.

For deeper implementation notes, use these companion documents:

- [../compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
- [../error-system.md](/home/matt/nerd/docs/error-system.md)
- [../type-system.md](/home/matt/nerd/docs/type-system.md)
- [../lsp.md](/home/matt/nerd/docs/lsp.md)
- [../testing.md](/home/matt/nerd/docs/testing.md)

## C output

`src/compiler/cgen/cgen.c` implements the optional HIR-to-C backend. It consumes
the checked whole-program HIR and semantic type tables, canonicalises C types
across modules, and emits forward declarations before function bodies. Separate
arenas own output, function bodies, declarations, and temporary names. Generated
C embeds `data/nrt.c` and `data/ncg.c`, so generation works after installation
without access to the compiler source tree. LLVM remains the default backend.
`build --copts` is an artifact-free query after whole-program checking: it reuses
the backend's library filtering to print Clang arguments on one space-separated line. It can
also accompany C generation. C object/library modes omit the executable entry
point, emit validated root C exports, and initialise modules through a library
constructor. The static-library options describe object compilation; archiving
is a separate tool invocation.
C layout identity is separate from semantic type identity so generic dispatch
can distinguish, for example, slices of different element types. Expression
temporaries preserve evaluation order; explicit exit cleanup handles `defer`,
boxes and varargs. Differential tests compile the generated C at two optimisation
levels and compare execution with LLVM. See
[../compiler-pipeline.md](../compiler-pipeline.md) for the backend contract.

## Overall Shape

The repository contains:

- `src/core`
  Low-level utilities shared by almost everything else.
- `src/object` and `src/table`
  Shared data helpers built on `core`.
- `src/cli`
  Schema-driven command-line parsing.
- `src/compiler`
  The compiler front end, HIR, LLVM generation, error system, and commands.
- `src/lsp`
  The language server built on the compiler front end and CST.
- `build/test.py`
  The repository regression test runner.
- `data/nrt.c`
  C implementation of runtime helpers linked into generated executables.
- `tests`
  Language, error, formatter, and LSP test inputs.

The main executable entry point is [src/nerd.c](/home/matt/nerd/src/nerd.c).

## Reading Order

For most compiler work:

1. [src/nerd.c](/home/matt/nerd/src/nerd.c)
2. [src/compiler/compiler.h](/home/matt/nerd/src/compiler/compiler.h)
3. [src/compiler/build/front/front.c](/home/matt/nerd/src/compiler/build/front/front.c)
4. [src/compiler/build/back/back.c](/home/matt/nerd/src/compiler/build/back/back.c)
5. the subsystem you are changing

For editor tooling work:

1. [src/lsp/lsp.c](/home/matt/nerd/src/lsp/lsp.c)
2. [src/lsp/document.c](/home/matt/nerd/src/lsp/document.c)
3. [src/lsp/hover.c](/home/matt/nerd/src/lsp/hover.c)
4. [src/compiler/cst/cst.c](/home/matt/nerd/src/compiler/cst/cst.c)

For tests:

1. [tests/README.md](/home/matt/nerd/tests/README.md)
2. [docs/testing.md](/home/matt/nerd/docs/testing.md)
3. [build/test.py](/home/matt/nerd/build/test.py)

## Execution Flow

The `nerd` executable is mostly orchestration:

1. parse CLI input
2. build a command-specific config struct
3. run compiler, LSP, or test commands

The compiler front end currently runs:

1. lexing
2. AST parsing
3. semantic analysis
4. HIR generation

The back end currently runs:

1. optional save of generated HIR
2. LLVM IR generation
3. optional save of generated LLVM IR
4. release IR optimisation through `opt`, followed by `llc` object generation
5. linking with the embedded Nerd runtime through the platform LLVM linker

Both `opt` and `llc` receive the same explicit native target triple, selected
from the compiler's architecture and host OS (x86-64/AArch64 on Linux, Windows
or macOS). `opt` must select the target data layout before optimization;
otherwise its generic layout can disagree with `llc` about aggregate field
offsets. This caused release-only corruption of Pixels' layer data pointer.
The headless pixel-layer regression runs allocation, painting and cleanup in
debug/release mode through the production toolchain, including parallel renders.

`nerd doctor` checks the LLVM executables and host SDK, then exercises this
pipeline with a temporary executable. Nerd does not invoke Clang; C emission is
an independent compatibility output. See [toolchain setup](../toolchain.md) for
host dependencies and [the pipeline](../compiler-pipeline.md) for artifact policy.

Every non-core module receives semantic proxy declarations for every public
export from `mods/core.n`; there is no symbol-name whitelist and no explicit
source import. The executable backend locates core's private lifecycle functions
by their owning module and generated function indices. Its host entry wrapper
runs module global initialisers, `core_init`, user `main`, then `core_done` on a
normal return. Debug runtime shutdown releases core-owned storage before walking
the live heap and arena lists and reporting leaks to standard error.

## Important Architectural Rules

- Keep the AST syntax-only.
- Prefer semantic side tables over larger AST nodes.
- Keep compiler stages explicit rather than smearing semantic logic into the parser.
- Keep renderers stable for tests and keep dumpers human-oriented.
- Treat `ErrorInfo` as the source of truth for diagnostics rather than terminal text.
- Emit fixed-size LLVM local and scratch `alloca` instructions through the
  entry-block allocation helper. An `alloca` emitted in a repeated basic block
  consumes more stack each time that block executes.
- The LLVM addressed-local prepass must traverse control-flow scrutinees as well
  as their branches and guards, so locals whose addresses are conditionally
  used are materialised and initialised on every path.

## Key Data Products

- `Lexer`
  Tokens, symbol handles, and source mapping.
- `Ast`
  Compact syntax tree nodes.
- `Sema`
  Semantic declarations, type rows, dependency edges, and AST-indexed side tables.
- `Hir`
  Semantically checked lowered program used by the executable backend.
- `LLVM`
  Generated LLVM IR text and backend lowering state.
- `Cst`
  Concrete syntax tree used mainly for formatter and LSP tooling.

The tooling CST accepts an accidental C-style statement terminator inside a
block so the formatter can remove it while retaining structured layout for the
rest of the block. The compiler AST remains authoritative for rejecting that
semicolon as invalid Nerd syntax.

Sema records the expected type of each checked `on` pattern in an
AST-pattern-indexed side table. Nested payload patterns therefore retain their
own contextual type independently of the outer scrutinee, which editor features
can use for enum completion while a branch pattern is being edited.

When a value-producing `on` consists of ordinary braced statement branches,
Sema diagnoses the void branch at the `on` expression instead of allowing the
later variable-storage check to report a void local. Value-bearing braced
branches must use the `${ ... }` expression-block form with `break <value>`.

The formatter keeps short boolean `on` expressions on one line. When the full
expression exceeds the wrap width, it places the `=>` and `else` branches on
separate lines, indented once from the line containing `on`, and aligns their
result expressions after the branch markers.

Completion also classifies cursor positions in incomplete `plex` declaration
bodies directly from the open source buffer. Field-name positions suppress the
general symbol fallback, while field-type positions retain only type
declarations; source scanning keeps this distinction available before a closing
brace has been entered and semantic analysis can finish.

For repaired member completion, Sema retains the declarations, locals, and type
facts established before a later declaration-type error. Completion can then
use the receiver facts established at the cursor, so an unrelated error later
in the same function does not hide members whose receiver type was already
known. Other LSP analyses retain their normal partial-result policy.

If ordinary symbol hover has no semantic result, hover performs a private
analysis that retains facts established before a later declaration-type error.
This recovers local and imported member hover without publishing diagnostics
from incomplete semantic validation.

Local hover resolves declaration tokens directly through each `SemaLocal` as
well as through AST binding and reference nodes. This covers locals such as
`for in` item and index bindings whose declaration tokens belong to the loop
metadata rather than standalone binding nodes.

Integer range membership has dedicated CST, AST, and HIR binary nodes. Sema
contextualises the bracketed range bounds from the tested integer type. LLVM
lowering evaluates the tested value once and short-circuits the upper-bound
comparison when the lower-bound comparison fails. In loop headers, the parser
reserves bare `for name in expression` for iteration; a membership condition is
therefore written in parentheses.

Before unannotated locals are materialised, Sema collects concrete numeric usage
constraints from known call signatures, assignments, return annotations,
arithmetic, and comparisons. A temporary table indexed by literal AST node stores
constraints; traversing local initialisers and tuple projection paths shares them across
aliases. A fixed-point scan propagates newly discovered constraints before
constructing local types and defaulting unconstrained tuple fields. Conflicting
constraints report a type mismatch. Explicit types remain boundaries, and generic
bodies and unresolved overloads are left to their existing inference paths.
Callable signatures are resolved without inferring ordinary function bodies.

The same constraint table records nominal types for targetless plex/union
literals. Pointer parameters pass their pointee constraint through address
expressions, and record field types constrain nested literals and numeric
aliases. These literal targets are collected before local initialisers are
checked, so aliases share a consistent target and incompatible record uses are
reported before either use wins. Ordinary literal checking retains responsibility
for field names, defaults, missing fields, and bitfield ranges.

The tuple expression parser collects child nodes in a temporary array before
appending them to the AST tuple item table. Nested tuples therefore cannot
interleave their entries with the parent tuple's contiguous item range.

Sema also collects explicit numeric cast targets from compound-assignment
operands. An untyped literal initializer
can therefore adopt a later explicit numeric type without requiring the rest of
the operand to be inferred before loop bindings and other locals are ready.

## File Families

Trait requirement parsing validates that every outer function parameter has a
name. Function definitions and FFI declarations also require names; standalone
function types may omit them. Sema's unused-local validation treats method
receivers like other parameters, including the underscore-name rules. Trait
requirement parameters are signatures rather than runtime locals.
Function and FFI signature parsers collect their parameters in a temporary
array before appending the contiguous outer parameter range. Nested function
parameter and return types therefore cannot interleave their parameters with
the enclosing signature.

- `src/compiler/lexer`
  Tokenisation and source-position helpers.
- `src/compiler/ast`
  Syntax parsing, AST utilities, and AST dumping.
- `src/compiler/sema`
  Name resolution, dependency ordering, constant folding, and type analysis.
- `src/compiler/hir`
  HIR data model, lowering, and rendering.
- `src/compiler/llvm`
  LLVM IR emission.
- `src/compiler/error`
  Structured diagnostics and renderers.
- `src/compiler/format`
  Formatting rules and formatter output.
- `src/lsp`
  LSP message handling and editor-facing features.

HIR expressions that represent implicit runtime work retain the source path and
line of the source construct that caused them. In particular, the synthetic
array expression used to allocate a capacity-bearing dynamic-array local uses
the local declaration location, which LLVM passes to the runtime allocator for
leak diagnostics.

Dynamic-array heap storage pads its three-word header to 32 bytes before the
first element. Both LLVM lowering and the compatibility C helper preserve the
allocator's 16-byte alignment across allocation, growth, reserve and pointer
conversions. The former 24-byte prefix misaligned LLVM enums with wide payloads;
optimized Windows Dungeon then faulted on an aligned SIMD store while appending
terminal input events. Regression checks exercise these stores and their values
in debug/release targets at jobs 1/4, including explicit capacity and reserve.

Implicit fixed-array-to-slice call arguments are lowered from the address of the
original HIR array expression. This preserves the language's borrowed-view
semantics and avoids directing mutations through a copied LLVM temporary.
Generic argument inference applies the same fixed-array-to-slice compatibility
while binding element parameters. It defers empty array arguments until other
arguments or the expected return type have supplied their generic element type,
then checks those literals with the substituted parameter type.

Method receivers declared as `^Self` are lowered by taking the address of the
receiver value. When `Self` is a slice and the call receiver is a fixed array,
method resolution applies the normal fixed-array-to-slice borrow first and then
passes a pointer to that slice value. Inside the method, the receiver therefore
retains its declared pointer type; `self^[i]` explicitly indexes the slice,
while `self[i]` uses Nerd's ordinary pointer-indexing semantics.

The AST and CST Pratt parsers classify a caret attached to its left operand and
followed by `[` as postfix dereference rather than bitwise XOR. This allows an
index or slice postfix to follow immediately, as in `ptr^[i]`, while preserving
spaced `lhs ^ rhs` as bitwise XOR. The formatter emits the chained postfix form
without redundant parentheses.

When an expression-bodied function omits its return annotation, semantic
analysis records any postfix `?` failure channel while inferring the body. It
then wraps the inferred success type in the corresponding optional or result
type. Nested expression-function inference saves and restores this context, and
multiple propagations must use compatible failure channels.

Inferred expression returns also unwrap atomic storage to its ordinary value
type, matching the language's implicit atomic-load semantics.

The install recipe copies the standard modules before atomically replacing the
watched compiler executable. The VS Code extension also stages those companion
modules beside its uniquely named server copy. This keeps implicit `core`
resolution independent of the staged executable's location and ensures an
editor-triggered LSP restart cannot observe a missing or partially installed
`core.n` module.

The parser recognises a same-line `arena name` sequence as a reversed built-in
type declaration and reports Nerd's name-first declaration forms. The formatter
preserves that malformed sequence on one line so formatting cannot obscure the
source relationship used by the diagnostic.

While Sema checks a plex bitfield value, it records the root value node, field
symbol, and expected value type as temporary mismatch context. The generic type
mismatch path uses that context only for the root expression, adding the
bitfield note without annotating unrelated errors inside nested expressions.

FFI and intrinsic signatures retain compulsory parameter names in their normal
`AstParam` rows. The ABI-facing semantic function type remains name-independent,
while hover and signature help read the AST names for editor-visible labels.

The multiline-array formatter associates an element's trailing comment with
the separating comma when one is present, or with the element's final token
otherwise. It emits and consumes that comment alongside the reconstructed
element so formatting cannot discard comments between array values.
The token-stream recovery formatter applies the same rule when a later syntax
error prevents CST formatting of the file.

The missing-plex-fields code action resolves an empty literal from either its
inline type annotation or the type of an assignment target. This allows the
action at the cursor in both `value: Type = { }` and a later `value = { }`
assignment.

Deferred variable and constant declarations reuse the formatter's compact
header-item rendering. This preserves declaration syntax after `defer` without
routing statement-only CST nodes through expression rendering.

Typed plex-literal lookahead recognises a shorthand field followed by `...`,
matching the prefix-literal parser and the formatter's shorthand field output.

Plex bit-field blocks remain grouped in the CST and AST so the formatter can
reconstruct the storage block and align its `:` separators. Semantic analysis
flattens named bits into the record's ordinary member arrays and records
parallel width, offset, and group-start metadata; `_` padding advances the
offset without creating a member. Enum-typed bits store the enum as their
logical member type and keep the block's unsigned integer in parallel physical
storage metadata. The LSP uses the grouped AST for declaration locations,
logical types, and documentation, while semantic member completion sees the
flattened fields.

Semantic dependency ordering excludes a function's references to its own
callable symbol. Function declarations establish that symbol before their
bodies are ordered, so direct recursion is not a declaration-initialisation
cycle. Other self-references continue through the normal cycle checks.

Semantic pointer comparison accepts compatible pointee types, including a
`^void` side, for equality and ordering. HIR retains the ordinary comparison
operation and operand types; LLVM selects unsigned `icmp` predicates for
ordered pointer comparisons.

Generic implementation parameters are parsed from an explicit list immediately
after `impl`, for example
`impl [T] Display for Box[T] where T: Display { ... }`. Keeping declaration
separate from target matching permits parameters used only in member signatures
or bodies, such as `impl [T] Stack[i32]`. Method resolution first binds any
parameters present in the receiver target, then binds remaining parameters from
method arguments before instantiating the member.

## Related Documents

- [FORMAT.md](/home/matt/nerd/docs/overviews/FORMAT.md) for formatter rules
- [../manual/appendix-a-syntax-reference.md](/home/matt/nerd/docs/manual/appendix-a-syntax-reference.md)
  for the source-level syntax reference
- [build-directives.md](/home/matt/nerd/docs/overviews/build-directives.md) for build-system metadata
# Atomics

Semantic types represent `atomic[T]` with `STK_Atomic` and retain the element
type in `first_param_type`. Semantic analysis validates scalar/thin-pointer
elements and treats ordinary reads and assignments as element transfers. Sema
attaches an internal operation identity to the inherent methods supplied by
`std.atomics`; HIR therefore contains explicit atomic load, store, exchange,
fetch, and compare-exchange expressions rather than rediscovering operations
from method names. The standard-library declarations carry explicit
`compiler_intrinsic("atomic.*")` identities which Sema validates against an
`atomic[T]` target. LLVM lowering emits the corresponding `load atomic`, `store
atomic`, `atomicrmw`, and `cmpxchg` instruction with the order already resolved
to a compile-time value. Function parameters marked `::` are recorded in the
AST/CST and checked against the constant-expression model before HIR generation.
Their canonical value tuples form specialisation identities; specialised HIR
substitutes the constants and omits them from the runtime ABI.

The LSP keeps editor-facing metadata for compiler-provided dynamic-array,
arena, and atomic methods in `src/lsp/builtin.c`. Hover and signature help share
that catalogue, which supplies call-site parameter names, documentation, and
signatures with the implicit method receiver removed. The same catalogue models
the built-in `arena(...)` construction syntax, since it has no source-level
function declaration from which the LSP could recover parameters.

Arena allocation methods carry call-site `@file` and `@line` defaults into
`nrt_arena_alloc`. When a zero-initialised arena is first used, the runtime uses
that metadata while lazily initialising its storage, so leak reports identify
the allocation method call. An arena created explicitly with `arena(...)`
retains the constructor's source location instead.

### Compound functions

The AST records `fn { ... }` member syntax only. Semantic analysis registers
compound declarations, resolves dependency edges, rejects cycles and unsupported
members, and stores a flattened array of concrete declaration indices. It
constructs parameter-only effective signatures, including trailing-default
surfaces, and rejects declaration-time overlaps.

Call and expected-function contexts select exactly one concrete declaration.
Semantic node tables record both that declaration and its lowered symbol; these
tables are also snapshotted for generic caller instantiations. Imported public
compounds materialise local proxies for their signature set while retaining
source-module provenance and private implementation visibility. HIR generation
therefore receives only concrete function types and symbols. There is no HIR or
LLVM compound entity, symbol, wrapper, or dispatcher.

### Collection equality

Semantic analysis checks element equality recursively for arrays, slices, and
boxes. Selected custom element methods live in `Sema.equality_methods`, a
compact type/declaration side table. HIR converts these to typed callee
references in `Hir.equality_methods`, so LLVM does not resolve traits itself.
Imported methods are resolved against their source module and imported types.

LLVM retains `slice_eq` for integer and boolean sequences and emits typed,
short-circuiting loops for other elements. Nested comparisons operate on
borrowed element pointers. They do not consume owning boxes. Box comparisons
use `nrt_mem_size` to obtain element counts and distinguish nil from allocated
storage. Box count lowering divides allocation bytes by element storage bytes;
count-based constructors ensure the semantic `usize` type exists for lowering.

The generic-body validator follows receiver `Self`, pointer dereferences, and
indexing to the generic element type. Equality of slices and boxes requires
that element's `Eq` constraint; pointer identity and literal nil checks do not.
Both block and concise generic bodies are validated. Missing-constraint
errors highlight the operator and recommend an explicit `where` clause.

### Module library selection

`module_library_path` centralises library selection for compiler and LSP lookup:
`NERD_LIB_PATH` when defined, otherwise `mods` beside the executable. An empty
configured path disables library search. Explicit imports and implicit `core`
then search the invocation directory, without legacy install or source-root
fallbacks. Bare sibling imports retain their module-relative lookup.

LSP definition requests on import paths fall back to `module_resolve_path`
and return the resolved file at line zero directly. They do not require a
loaded program module view, so an earlier import failure cannot prevent
navigation to another resolvable module (including folder-module `mod.n`).
If that failure discarded the root AST, the request parses the open buffer
without loading its imports and resolves the module path from that scratch AST.

## Receiving Variadic C Calls

AST/CST function signatures retain the final named variadic binder separately
from fixed parameters. Semantic analysis binds it as an `is_variadic` parameter
of opaque `STK_VaList` type. HIR records `varargs_local_index` separately
from the fixed ABI parameter list.
`HIR_EXPR_VaNext`, `HIR_EXPR_VaCopy`, and `HIR_EXPR_VaFormat` represent the cursor
operations. LLVM emits a variadic definition and calls `llvm.va_start` on the
first member of a runtime-allocated `NrtVaList`. The host C compiler determines
`va_list` storage and argument traversal, avoiding hard-coded register offsets.

Copies are linked to the originating cursor. All allocations are released by
`nrt_va_done` after user defers, including early return and propagation paths.
The cursor may be borrowed by Nerd helpers but cannot escape via return,
assignment, addressing, or containers. Formatting uses two temporary `va_copy`
lists and preserves the input cursor. Fixed FFI `VaList` parameters on x86-64
carry `STF_FunctionCVaList` to distinguish native C parameter adjustment.

Runtime linker names consistently use `nrt_`; PIC runtime objects have hidden
visibility. Public root C functions receive plain aliases. Existing public
function bindings provide export-name overrides; generated implementations and
Nerd-only linkage aliases remain compiler-managed. Non-executable builds
validate exported signatures before generating artifacts.

The CST parser buffers each callable parameter list before appending it to the
shared parameter table. This preserves outer parameter names and types when
callback signatures contain nested parameter lists, including variadic callbacks.

Local aliases of built-in types, including `VaList`, are resolved as types.
HIR omits local type-alias declarations because they require no runtime storage.

### Result propagation and imported display methods

LLVM storage extraction treats a `void` result payload as a successful,
valueless expression. Propagating `void\Error` must continue the success path,
including later statements and deferred cleanup. It must not abort block
emission because the payload has zero storage bits.

Interpolation imports the canonical `core.Display` trait on demand when a
non-primitive value needs it. Imported `show` methods are resolved using the
implementation AST from their defining module, and lowered through their HIR
import; their AST indices are not indices into the caller's module.

## Compiler performance instrumentation

`NERD_PROFILE=1` emits JSON records on stderr for whole-program front-end module
phases, HIR, module LLVM rendering, LLVM combining, C rendering and each external
LLVM tool. Dependency records include implicit imports. The opt-in probes live
in `src/timing/timing.c` and avoid compiler arenas so active string builders are
unaffected. Thread CPU and elapsed wall time are separate; subprocess CPU and
peak process RSS come from the benchmark runner. Workers capture value-only
records in fixed result slots; the coordinator emits the stream. See
[profiling and benchmarking](../compiler-profiling.md).


Semantic inference and HIR lowering also traverse eager arithmetic trees with
explicit frames. Inference retains the expected type for each parent, derives
the right operand's expectation from the completed left type, and runs the same
result validation and diagnostic logic at each node. Traversal results are not
reused across inference contexts. HIR indices retain recursive postorder. Simple
operators take an allocation-free path; nested arithmetic uses storage proportional
to expression depth. This permits the 6,000-term regression in debug, release and
ASan without increasing the native stack. Other expression forms keep their
existing handling; this is not a universal arbitrary-depth guarantee.

LLVM emission walks nested eager arithmetic/bitwise binary expressions with an
explicit array of traversal frames. Each frame holds its HIR index and the
completed left operand while the right subtree is evaluated. This preserves
left-to-right side effects, parenthesization, pointer arithmetic and temporary
numbering without one native expression-emitter frame per binary operator.
Single operators with non-arithmetic children keep an allocation-free path.
The traversal array is freed on success or failure. Comparisons, range tests,
short-circuit operators and other expression kinds retain their existing
lowering; this does not eliminate recursion from every compiler phase.

The LLVM text combiner uses string hash maps for defined and already-declared
symbol membership, avoiding repeated linear scans as module/function counts grow.
It never iterates those maps for emission: module and line order still determine
output and the first retained external declaration. Maps own their key storage
and are freed after combination.

The LLVM text combiner reuses one line scratch arena per input module. Each
rendered line is copied into the combined output or named metadata builders
before scratch reset. The arena retains capacity for that input's longest line
and is freed at the end of the input, avoiding per-line virtual-memory churn.
Metadata remapping scans for numeric references but appends each unchanged span
in one operation. This preserves the remapped text while avoiding an arena
allocation and shared bookkeeping-lock acquisition for every unchanged byte.
Adjacent references, empty lines and trailing spans retain their old behavior.


Before LLVM emission, the backend builds a program-wide table of function-name
counts. Names come from each function's first HIR binding, preserving the
existing qualification rules: aliases count once, unbound functions do not
count, and repeated spellings within or across modules require qualification.
Counts saturate at two. `ProgramInfo.llvm_function_name_counts` borrows this
read-only table throughout module and sidecar rendering; the backend clears
the pointer and frees the table after successful or failed emission. C emission
and checking do not build it. Standalone HIR rendering without an index retains
the scan fallback. Index construction still resolves canonical bindings by
scanning each module's bindings once per function; repeated conflict queries
now look up the resulting spelling instead of rescanning the whole program.


Immediately before whole-program HIR generation, the front end builds line-start
arrays for source snapshots and mapped fragment buffers. `ProgramInfo` owns the
table; lexer and module layouts are unchanged. Equal buffer pointer/length pairs
share one index. HIR source locations and LLVM local debug locations use
`lex_indexed_offset_to_line_col`, which finds the buffer's index then binary
searches its line starts. Zero-based lines and byte columns, LF handling, EOF
positions and invalid-offset behavior match the original scanner. Fragment
mapping still happens before lookup. Other source buffers and standalone HIR
without a program index fall back to scanning. The table is read-only after
construction and freed by `program_info_done` before source snapshots are
released. Checking does not allocate an index. Entries use 32-bit byte offsets;
external fragments larger than that range retain scanning. Diagnostic, formatter
and LSP callers of the source-only API keep their existing behavior. Storage
grows with indexed line count, including separate fragment indexes when their
buffer views differ from the combined source.


Usage-context local-type inference filters AST node kinds before checking
whether nodes belong to disabled or generic bodies. Those scope predicates
scan AST ranges, so nodes that cannot contribute constraints bypass them.
Eligible nodes retain their traversal order and scope exclusions on every
fixed-point pass. Local-declaration constraints, convergence and final seeding
are unchanged; no inference results are cached.


Declaration collection filters non-declaration AST kinds before querying their
enclosing conditional bodies. Its FFI wrapper search resolves a binding's
payload and compares the exact target node before checking function scope.
These read-only checks avoid repeated AST scans for unrelated nodes while
preserving recursive traversal, first-wrapper selection and diagnostic order.


LLVM rendering lazily initializes three private scratch arenas per module:
temporary values, entry-block text and body text. Global initialization and each
function body reuse this set, resetting cursors before the next body. Declarations
without bodies do not initialize scratch. Entry/body text, including annotated
debug text, is copied into the module output before reset; function-local arrays
are still freed after each body. Debug metadata and module output use separate
longer-lived arenas. The scratch set is destroyed after the function loop,
before export wrappers, and is never shared across module tasks. Committed
capacity follows each arena's largest function in the module, rather than
reserving/committing/unmapping three arenas per function. This trades temporary
high-water retention within a module for fewer virtual-memory operations.

LLVM module outputs use fixed result slots, each with its own arena. The slot
array is fully sized before rendering; LLVM text and any alternate sidecar
render belong to that module's arena. Output paths, runtime glue and combined
LLVM belong to the coordinator arena. Combining copies the module text, so
render-result arenas and borrowed module views are released before writing
combined LLVM or invoking tools. Cleanup tolerates unstarted slots and already
released results. Module order, sidecar writes, timings and initialization-order
collection remain serial. `build --jobs N` dispatches module rendering into
these fixed slots; the default is one job.

Each result slot also owns an `ErrorContext`. A thread-local binding selects
the context used by the existing diagnostic APIs, with a default context for
ordinary callers. Contexts own message scratch, rendering mode, output settings,
last-rendered text and a deferred queue. Arenas are allocated lazily. Capturing
deep-copies diagnostic messages, references, notes, help and source/fragment
snapshots, so queued records survive scratch resets and input-storage release.

LLVM rendering binds the module context, captures diagnostics, restores the
previous binding and replays on the coordinator in module order. Replay consumes
the queue using the destination context's rendering and output settings; cleanup
also handles discarded queues and unstarted slots. The diagnostic renderer still uses the
global temporary arena and must remain on the coordinator. Fatal internal
compiler errors remain immediate process exits. Workers capture diagnostics;
the coordinator alone invokes the diagnostic renderer after joining workers.

Memory bookkeeping uses a statically initialized process-wide lock (SRW lock on
Windows, pthread mutex on POSIX). It protects counters, coherent snapshots and
the debug allocation list/index. Live and peak bytes remain process-wide, so a
block can be allocated, reallocated and freed by different threads after an
ownership handoff. Debug list removal is constant-time using previous/next
links; application-lifetime blocks remain accounted for but are excluded from
leak reports. Headers preserve `max_align_t` alignment for returned pointers in
both build configurations. The allocator releases the lock around libc
allocation calls; an in-flight reallocation is temporarily absent from debug
list queries.

Leak reporting uses libc output while holding the bookkeeping lock, avoiding
recursive allocation through Nerd's formatted-output buffer. Individual blocks
and arenas still require exclusive ownership or caller synchronization. Global
counter deltas include all concurrent work and cannot attribute memory to a
task. A separate set of thread-local activity counters counts events on the
executing thread, including frees and reallocations of handed-off blocks.
Its live/peak fields stay zero; those values are only meaningful process-wide.

`TimingProbe` compares thread-local activity snapshots around non-yielding
work that begins and ends on one OS thread. Nested probes include nested work;
future tasks must not migrate or execute unrelated queued tasks inside a probe.
`timing_probe_finish` returns a value-only `TimingProbeResult` with wall time,
thread CPU time, activity deltas and process-wide live/peak observations. It
does not render output or retain source pointers. LLVM result slots own primary
and optional sidecar records, finished before diagnostic replay. The coordinator
supplies labels/paths and emits records in stable module order. Other serial
callers use the existing finish-and-emit wrapper. Dependency records, human
timing tables and legacy memory-profile output still require coordinator or
serial use. Per-module wall times can overlap with multiple jobs: use their
start/end envelope for render elapsed time, not their sum. `NERD_PROFILE_LOCKS=1`
adds scoped thread-local acquisition counts/timings to enabled probes, preserving
nested settings. Two clock reads measure native-lock acquisition duration;
accounting/conversion occurs after unlock. Disabled scopes perform no extra
clock reads. This intrusive measurement includes uncontended overhead and
scheduler delays, so collect it separately from latency samples. The Windows
performance-counter frequency cache is thread-local to avoid lazy-init races.

Core worker primitives use pthread threads/conditions on POSIX and
`_beginthreadex` plus Windows condition variables on Windows. `Thread` is
zero-initialized storage owned by one coordinator. Its address and callback
arguments must remain stable from start through successful join; do not copy
or move a live thread. Failed creation resets the slot, duplicate starts fail,
and joins release/reset successfully joined slots. Joining unstarted or already
joined slots succeeds, allowing partial-start cleanup. Join failure retains the
handle so callers cannot silently free live worker storage. Workers are never
detached or asynchronously cancelled.

Condition waits release and reacquire the associated mutex. Callers protect a
predicate with that mutex and recheck it in a loop, including a stop predicate
for shutdown. The coordinator wakes stopped workers and joins them before
destroying synchronization objects or task input/result storage. These are
also used by the bounded LLVM task batch in normal builds with `--jobs N`.

LLVM render semantic snapshots own all ten mutable type arrays: types,
parameter types/symbols/values, braced-payload flags, four bitfield metadata
arrays and plex uses. `sema_materialise_type` can mutate these despite its
const parameter. Imported constants and default-argument expressions also use
private snapshots, with default snapshots allocated only when needed. The rest
of the program is borrowed read-only and must outlive every render task.
The internal concurrent-render harness checks serial/worker output identity,
input type-array fingerprints and serial reuse after worker cleanup, including
two workers rendering the same module. It joins all workers before releasing
the emission-scoped name index or input program. Production builds preserve
the same input lifetime until all task workers have joined.

`task_run` dispatches a finite range of task indices. It clamps the requested
execution slots (1–256, including the calling thread) to the task count. One
slot runs inline without native threads or synchronization allocation. Multiple
slots share a mutex-protected next-index counter and fixed stack-owned thread
slots. A condition-variable startup gate prevents any callbacks until every
worker starts successfully. Partial startup wakes and joins started workers
without executing tasks. Callback failure stops further dispatch; active tasks
finish before all workers are joined. No worker waits for another queued task.
A failed join aborts rather than returning with live pointers into stack storage.

`NerdBuildConfig.jobs` flows into `NerdArtifactConfig.jobs`; zero in an internal
configuration means the serial default. `build --jobs N` / `-j N` validates the
range before compilation. Each callback owns one module's arena, diagnostics,
primary LLVM text and optional alternate sidecar render. All parallel callbacks
finish before diagnostic/metric replay, sidecar writes, initialization-order
collection, combining, tool invocation or program teardown. The inline path
processes one result at a time so an early sidecar-write failure still stops
before rendering subsequent modules. Parallel mode may have completed later
modules by then; their results are safely discarded. The current LLVM callback
always completes or raises a fatal internal error; recoverable renderer errors
will need to return failure explicitly before task cancellation applies there.
Profiled parallel builds also capture callback entry/exit points in stable
result slots. After joining, the coordinator emits a scheduler record with
batch wall time, first-dispatch and drain intervals, and summed task dispatch
delays. These intervals include startup and waiting for earlier tasks; they do
not isolate queue-mutex contention or pure native-thread API costs. Startup
failure emits zero completed tasks before the normal error. Single-job mode
retains per-module profiles without a parallel-batch record.

C rendering, tool-option queries and `run` remain serial; `build --cgen` can
use the scheduled front end. There is no automatic memory budget or worker sizing;
the M8 adoption review retains one job by default. Batches with fewer than two
tasks run inline; explicit larger job counts remain bounded by available work.
Source-size thresholds and sharing imported semantic state are deferred because
measurements do not justify default parallel adoption. `build/test_jobs.py`
exercises production CLI parity and failure cleanup, including optional full-compiler
ThreadSanitizer/AddressSanitizer builds.

Record-literal semantic inference copies the enclosing record's type layout
before checking field expressions: recursive inference may grow and relocate
the type array. Keeping a pointer into that array caused a use-after-free
detected by AddressSanitizer on Quill.

### Front-end task ownership

`build --jobs N` also bounds module HIR lowering after all semantic analysis
and line-index preparation finish. Each task owns its module's HIR and captures
diagnostics and phase measurements; the coordinator publishes those records in
module order after joining. Compile-time HIR specialization selection is
thread-local. Foreign symbol interning writes only the destination lexer;
imported symbol reads use existing handles in stable reserved arenas, whose
data pointers and existing strings do not move. Verbose dumps and the legacy
process-wide memory report retain serial lowering. The default remains one job.

The loader can preparse resolved sibling imports into private source arenas and
front-end states. It registers and adopts those results only at their original
depth-first visit, so completion order cannot change module IDs, pragmas,
exports, or the first diagnostic. Pending parse failures are captured, not
printed. Source-loader callbacks, verbose output and legacy memory reporting
keep the serial loader. Cache cleanup also destroys unvisited results when an
earlier module fails. Folder-module expansion uses the parse entry's arena.

For scheduled builds, discovery records the original DFS checking order and
then freezes the registry. Dependency-ready semantic tasks exclusively own
their transitive import closure, including implicit core: generic checking can
append types, symbols and instantiations to imported modules. Overlapping
closures retain DFS order. Disjoint closures use task-local program views and
publish changed module records only after joining. The active type substitution
and semantic scratch arena are thread-local, with scoped task cleanup. Common
core imports currently serialize checking; this deliberately preserves generic
instantiation order rather than promising parallel checking for every graph.

The coordinator buffers phase records at their original DFS positions. Failed
scheduled attempts discard captured output and retry the original serial loader;
this preserves the first diagnostic even when discovery reaches a later parse
error before an earlier semantic error. Error-only global suggestions also use
the serial retry. Custom source loaders, partial-result tooling, verbose dumps
and legacy memory profiling keep the original serial front end.

### Imported generic identity

HIR keeps the semantic analyzer's selected specialization symbol on generic
symbol references and explicit type-argument indexes. LLVM emission resolves
imported specializations by that symbol's text in the source lexer; type IDs
are module-local and cannot be compared across caller and source tables.
Explicit specializations also resolve as function values, not just direct calls.
The front-end scheduling suite exercises distinct specializations of a shared
import, inferred calls and function values through LLVM and generated C, with
runtime checks at each worker count.

### Automatic worker ceiling (M9)

`build --jobs auto` resolves through `thread_available_cpu_count()` and
`task_auto_jobs()` to `max(1, min(256, floor(logical CPUs / 2)))`. The calling
thread counts as a worker. This opt-in ceiling flows through the existing
front-end and LLVM task paths; batch readiness still limits actual concurrency.
Numeric jobs remain explicit overrides even under restricted affinity. Omitted
jobs remain one until adaptive dispatch and native performance gates pass.

Linux counts the calling thread's allowed CPUs using `sched_getaffinity`, growing
the CPU mask for sparse CPU IDs and large machines. Allocation/query failure
falls back to one (the mask-growth bound is 1,048,576 CPU IDs). Windows counts
`GetProcessAffinityMask` bits, conservatively restricted to its primary processor
group on machines with multiple groups. Other supported POSIX platforms query
online CPUs with `sysconf` when available, otherwise use one. CPU quotas,
Windows CPU Sets/job-object CPU budgets and macOS affinity are not accounted
for; this is not a memory budget. Windows/macOS paths require native validation
before default adoption. The platform-specific Windows behavior follows the
[GetProcessAffinityMask contract](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getprocessaffinitymask).

Core lifecycle tests cover rounding, zero/one-CPU fallback and the 256-worker
cap. Production jobs tests restrict inherited affinity to exercise available-CPU
counts, verify numeric overrides and the unchanged serial default, and compare
LLVM/C output and executable results. M10 adds work estimation and dispatch
thresholds below; shared semantic ownership is unchanged.

### Adaptive dispatch experiment (M10)

Automatic mode retains its CPU ceiling separately from numeric overrides.
`compiler/build/schedule.h` collects saturating work estimates: source bytes for
prefetched parses, AST nodes for HIR, and HIR expressions plus statements for
LLVM rendering. `task_jobs_for_work` limits a batch by its task count, ceiling,
total work and work outside its largest task. Small or dominated batches run
inline. Numeric jobs bypass this policy. Shared semantic closures retain their
existing exclusive ownership rule.

The second opt-in calibration uses grains of 16,384 source bytes, 2,048 AST
nodes and 1,024 HIR nodes. These are experimental policy parameters, not universal
crossover claims or a memory budget. `NERD_PROFILE=1` reports
`scheduler-policy` records with phase, ceiling, selected jobs and work estimates.
Front-end decisions are buffered with phase results so a discarded speculative
load does not leak profile records. Workers still join before publication;
there is no persistent pool. Omitted jobs remains one pending the adoption gate.
