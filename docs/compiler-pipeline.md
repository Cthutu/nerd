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
4. invokes clang on one combined LLVM IR input

The current executable back end is LLVM, so the user-facing compiler flow is
effectively:

`Nerd source -> lexer -> AST -> sema -> HIR -> LLVM IR -> native binary`

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

The backend renders each module in memory, writes `.ll` sidecars only when
requested with `--llvm`, concatenates the embedded LLVM prelude, generated
modules, generated epilogue wrapper, and init wrapper into one temporary
`.link.ll` file, then invokes clang on that single input. Temporary link inputs
are removed after successful builds.

## Renderers And Dumpers

The codebase distinguishes between:

- renderers
  stable text intended for files and test comparisons
- dumpers
  human-oriented diagnostic output for local inspection

That split shows up across the compiler, especially in HIR and LLVM generation.
