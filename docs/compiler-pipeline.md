# Compiler Pipeline

This document describes the current end-to-end compiler flow and the main data
that moves between stages.

## Front End

The front end is orchestrated in
[src/compiler/build/front/front.c](/home/matt/nerd/src/compiler/build/front/front.c).

The current stage order is:

1. `lex`
2. `ast_parse`
3. `sema_analyse`
4. `hir_generate`

The front end returns `FrontEndState`, which contains:

- `Lexer`
- `Ast`
- `Sema`
- `Hir`

This ordering is important. The parser stays syntax-only, semantic analysis
resolves names and types, and HIR generation lowers from semantic results rather
than raw syntax guesses.

## Back End

The back end is orchestrated in
[src/compiler/build/back/back.c](/home/matt/nerd/src/compiler/build/back/back.c).

It currently:

1. optionally saves HIR
2. renders LLVM IR from HIR
3. optionally saves generated LLVM IR sidecars
4. builds one combined LLVM IR link input
5. invokes clang on one combined LLVM IR input plus that runtime object

The current executable back end is LLVM, so the user-facing compiler flow is
effectively:

`Nerd source -> lexer -> AST -> sema -> HIR -> LLVM IR -> native binary`

The backend is intentionally split into small pieces:

- `back.c`
  owns artifact policy, module iteration, temporary paths, external link flags,
  and process orchestration.
- `llvm_runtime.c`
  owns the embedded runtime object and generated runtime glue such as the `init`
  wrapper and tiny C-compatible `main` wrapper.
- `llvm_text.c`
  owns textual LLVM concatenation and declaration filtering for the combined
  input.

## Lexer

The lexer owns:

- tokenisation
- symbol interning handles
- source offsets and line/column mapping

`Lexer` is the source of truth for converting offsets into user-facing spans.
Both the error system and the LSP rely on lexer position helpers.

## AST

The AST is compact and arena-friendly. The project explicitly prefers keeping
AST nodes lean and moving semantic facts into side tables.

The AST should answer syntax questions such as:

- what construct was parsed
- which child nodes belong to it
- which token anchors the node

It should not become the place where semantic meaning is stored.

Function bodies and nested block statements are represented structurally in the
AST, but scope ownership is still semantic data. A block node describes its
statement range; semantic analysis decides which declarations are visible.

## CST

The CST is the source-preserving syntax product for formatter and editor
tooling. It is intentionally separate from the compact compiler AST for now.
The accepted boundary is:

- AST is compiler-facing and compact.
- CST is tooling-facing and source-oriented.
- Shared syntax classification, token ranges, and construct predicates belong
  in common helpers instead of feature-specific formatter/LSP code.

This keeps trivia and tolerant tooling concerns out of the compiler AST while
still requiring both parsers to share vocabulary for binding-like constructs,
statement-like constructs, ranges, and cursor contexts.

## Semantic Analysis

The semantic pass in [src/compiler/sema/sema.c](/home/matt/nerd/src/compiler/sema/sema.c)
currently does several jobs:

- collect top-level declarations
- predeclare built-in functions
- resolve symbol references
- collect dependency edges between declarations
- order declarations safely
- infer and check types
- fold integer constants where possible

The output is a `Sema` object built mostly from compact side tables keyed by
AST node index or declaration index.

Local variable scopes are also semantic side tables. Each function body creates
a root local scope, and nested block statements create child scopes. Lookup is
lexical and declaration-order aware.

The front end now distinguishes semantic sub-products:

- declaration facts: top-level declarations, imports, exports, and declaration
  source anchors
- binding facts: source bindings, references, lexical scopes, locals, and
  source navigation facts
- type facts: best-effort or checked type rows, method facts, and type-derived
  completion/hover/signature information

These are currently backed by the same `Sema` tables, but they are separate
readiness contracts. Batch compilation requires checked semantic success before
HIR generation. Editor tooling may use declaration and binding facts from a
partial analysis and request type facts only when the feature needs them.

## HIR Generation

HIR lowering in [src/compiler/hir/gen.c](/home/matt/nerd/src/compiler/hir/gen.c)
uses the semantic output to produce:

- global declarations
- init-time work
- function bodies
- assignments, locals, calls, arithmetic, and returns
- structured blocks, loops, `on`, `defer`, and expression-valued control flow
- explicit string-builder operations for interpolated strings

HIR is the semantically checked middle layer. It is intentionally target-agnostic
at the language level: functions, types, values, imports, exports, locals, and
control-flow constructs are represented as compiler facts rather than as C or
LLVM spelling.

HIR also has a stable textual representation for tests. The text is derived
from the in-memory HIR; it is not the shape the data structures have to use.

## LLVM Generation

The LLVM back end translates HIR to textual LLVM IR in
[src/compiler/llvm/llvm.c](/home/matt/nerd/src/compiler/llvm/llvm.c).
Generated LLVM IR remains a tooling artefact as well as the executable backend
input, so stability matters for tests.

The naming rule to remember is:

- Nerd-visible bindings keep the `$` prefix and are emitted as LLVM aliases
  such as `@"$main"`
- generated implementation names such as `@fn.N` are compiler internals

The compiler build produces the runtime object before compiling `nerd` itself.
`src/nerd.c` has a build directive that compiles `data/nrt.c` to
`_obj/runtime/nrt.o`; `llvm_runtime.c` embeds that object with `#embed`.

For a normal program build, the backend renders each module's LLVM IR in memory.
It writes module `.ll` sidecars only when requested with `--llvm`. Executable
builds then concatenate:

1. each generated module
2. the generated runtime `main` wrapper
3. the generated `init` wrapper for modules with global initialisation

The concatenated text is written to one temporary `<output>.link.ll` file and
the embedded runtime object is written beside it as `<output>.nrt.o`. The
backend then invokes clang on both inputs. Using a single generated LLVM input
avoids backend-specific C dependency ordering and lets LLVM resolve
function/type ordering within the module.

The combiner removes declarations that are satisfied by definitions or aliases
inside the combined input, and keeps one declaration for unresolved external
symbols such as libc calls. This matters because clang accepts separate `.ll`
inputs that each declare the same symbol, but rejects one textual LLVM module
that both declares and defines the same symbol.

Temporary link inputs and runtime object copies are removed after successful
builds. Module sidecars are also removed unless `--llvm` requested them for
inspection. Failed builds retain generated files where possible so the failing
LLVM can be inspected locally. Backend tool failures report the exact command,
the generated LLVM path, the runtime object path, and the first captured tool
output.

The current toolchain contract is textual LLVM IR plus clang. The compiler does
not currently invoke `llvm-as`, `llc`, or `opt` directly. That keeps the install
surface small while still allowing a future measurement-backed switch to LLVM
CLI tools or bitcode if clang startup or textual parsing becomes a bottleneck.

LLVM generation now has an explicit layout context for the current target
contract. The initial context still describes the existing 64-bit assumptions:
opaque `ptr`, pointer-sized integers as `i64`, string and slice values as
`{ ptr, i64 }`, dynamic-array headers as `{ ptr, i64, i64 }`, and enum tags as
`i64`. The context is intentionally conservative: it centralises the choices
the backend already made before broadening target support.

Use `--timing` with `nerd build` or `nerd run` to print phase timings. Back-end
timings use wall-clock time so the external clang invocation is included in the
`link executable` phase.

Runtime helpers that exchange Nerd strings use a stable pointer/scalar ABI
rather than C by-value structs. Generated LLVM stores `{ ptr, i64 }` string
values into stack slots and passes pointers to runtime helpers such as
`string_eq`, `to_string$...`, `string_builder_append_string`, and
`nerd_assert`. This keeps the generated LLVM independent of platform-specific C
aggregate calling conventions.

Runtime helper declarations are emitted from declaration tables in
`src/compiler/llvm/llvm.c`, rather than from ad hoc text blocks. The tables are
the current backend source of truth for helper return types and LLVM parameter
spelling.

## Renderers And Dumpers

The codebase distinguishes between:

- renderers
  stable text intended for files and test comparisons
- dumpers
  human-oriented diagnostic output for local inspection

That split shows up across the compiler, especially in HIR and LLVM generation.
