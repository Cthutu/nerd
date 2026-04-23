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
4. `ir_generate`

The front end returns `FrontEndState`, which contains:

- `Lexer`
- `Ast`
- `Sema`
- `Ir`

This ordering is important. The parser stays syntax-only, semantic analysis
resolves names and types, and IR generation lowers from semantic results rather
than raw syntax guesses.

## Back End

The back end is orchestrated in
[src/compiler/build/back/back.c](/home/matt/nerd/src/compiler/build/back/back.c).

It currently:

1. generates C from IR
2. optionally saves generated C
3. optionally invokes the native compiler

The current code generator is a C back end, so the user-facing compiler flow is
effectively:

`Nerd source -> lexer -> AST -> sema -> IR -> C -> native binary`

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

## IR Generation

IR lowering in [src/compiler/ir/gen.c](/home/matt/nerd/src/compiler/ir/gen.c)
uses the semantic output to emit:

- global declarations
- init-time work
- function bodies
- assignments, locals, calls, arithmetic, and returns
- explicit block start/end markers for nested scopes
- explicit string-builder operations for interpolated strings

The current IR is intentionally straightforward. It acts as the bridge from
semantic structure to stable C emission.

The IR is also becoming self-contained. `IrValue` carries its semantic type, and
`Ir` owns copies of the semantic type rows plus explicit records for globals,
functions, and locals. Back ends should prefer those IR facts over reaching back
into semantic side tables. This keeps the representation suitable for future
non-C back ends and VM-style execution.

## C Generation

The C back end translates IR operations to emitted C. Generated C remains a
tooling artefact as well as the current executable back end, so stability
matters for tests.

The naming rule to remember is:

- Nerd-visible names become C symbols with a leading `$`
- hidden runtime/compiler helpers do not

## Renderers And Dumpers

The codebase distinguishes between:

- renderers
  stable text intended for files and test comparisons
- dumpers
  human-oriented diagnostic output for local inspection

That split shows up across the compiler, especially in IR and C generation.
