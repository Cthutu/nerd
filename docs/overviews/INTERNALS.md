# Internals

This document is the high-level map of the current codebase. It points at the
main subsystems and tells you where to read next.

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

For deeper implementation notes, use these companion documents:

- [../compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
- [../error-system.md](/home/matt/nerd/docs/error-system.md)
- [../type-system.md](/home/matt/nerd/docs/type-system.md)
- [../lsp.md](/home/matt/nerd/docs/lsp.md)
- [../testing.md](/home/matt/nerd/docs/testing.md)

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
4. native compilation through clang, linking the embedded Nerd runtime object

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

Before unannotated locals are materialised, Sema also collects explicit numeric
cast targets from compound-assignment operands. An untyped literal initializer
can therefore adopt a later explicit numeric type without requiring the rest of
the operand to be inferred before loop bindings and other locals are ready.

## File Families

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

Implicit fixed-array-to-slice call arguments are lowered from the address of the
original HIR array expression. This preserves the language's borrowed-view
semantics and avoids directing mutations through a copied LLVM temporary.
Generic argument inference applies the same fixed-array-to-slice compatibility
while binding element parameters. It defers empty array arguments until other
arguments or the expected return type have supplied their generic element type,
then checks those literals with the substituted parameter type.

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
