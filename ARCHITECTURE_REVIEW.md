# Architecture Review

This document records architecture review discussions, agreements, open
questions, and follow-up decisions for Nerd. It is intentionally a living
document: when a question is settled, move it from an open question into an
agreement or decision record.

## Context

The current documented pipeline is:

```text
lexer -> AST parser -> sema -> HIR -> LLVM IR -> clang
```

Important current constraints from the roadmap and developer docs:

- The AST stays compact and syntax-oriented.
- Semantic facts live in arena-backed side tables keyed by AST nodes,
  declarations, symbols, locals, scopes, and type rows.
- The CST owns source-preserving tooling such as formatting and editor-facing
  token structure.
- The LSP is currently thin over the compiler front end and reparses the whole
  edited document after changes.
- `just test` is the full regression gate and includes language, error,
  formatter, LSP, and command tests.

Current implementation hotspots visible during the review:

- `src/compiler/sema/sema.c` is large and owns many front-end responsibilities:
  declaration collection, symbol resolution, dependency ordering, type checking,
  constant folding, generic instantiation support, method handling, and side
  table population.
- `src/compiler/format/format.c` is also large and carries many layout-specific
  cases directly.
- `src/lsp/completion.c` has extensive semantic, AST, repaired-source, and
  module-fallback logic to keep completion working while source is incomplete.
- LSP document analysis sets `keep_partial_results = true`, skips HIR
  generation, and tries to retain partial compiler state for editor features.

Active decision records:

- `review/decisions/0001-hir-and-backend-boundary.md`: HIR boundary and
  retired IR/C backend migration record.
- `review/decisions/0005-llvm-sidecar-from-hir.md`: LLVM generation from HIR
  and the current executable backend contract.
- `review/decisions/0006-memory-ownership-strategy.md`: current allocation
  ownership policy.
- `review/decisions/0010-target-layout-support.md`: current 64-bit host target
  contract and unsupported cross-target/aggregate FFI limits.

## Final Closeout

The architecture review has converged on this current build pipeline:

```text
source -> lexer -> AST parser -> sema -> HIR -> LLVM IR -> clang + nrt.o
```

HIR is the durable checked middle layer. It is semantically typed,
name-resolved, target-agnostic, and rendered as stable text for tests. LLVM IR
is now the executable backend input and the backend comparison target; the
legacy custom IR and C generator are no longer part of the build path.

The chosen boundary is:

- lexer owns tokens, comments in format mode, source mapping, and interned
  symbols
- AST owns compact syntax for the compiler
- CST and formatter trivia own source-preserving tooling structure
- sema owns declaration collection, name resolution, type checking,
  diagnostics, and semantic side tables
- semantic readiness is split into declaration facts, binding facts, and type
  facts so editor features can use partial semantic products deliberately
- AST and CST remain separate products for now; shared syntax helpers and
  recovery fixtures are the mitigation against parser/tooling drift
- HIR owns lowered checked program structure: nameless entities, explicit
  bindings, modules, imports, exports, globals, init work, functions, locals,
  control flow, aggregates, strings, dynamic arrays, and runtime operations
- LLVM lowering owns target text, generated implementation names, Nerd-visible
  `$` aliases/wrappers, local CFG construction, phis, runtime glue, and link
  input assembly
- LSP consumes named source/token/syntax/semantic views and must tolerate
  partial products instead of assuming a successful full build

Settled choices:

- Nerd source bindings do not define entity identity. HIR entities use stable
  generated ids, and bindings point at them.
- LLVM keeps Nerd-visible names as `$` aliases or wrappers while generated
  implementation symbols use internal names such as `@fn.N`.
- `for item in collection` binds `item` as a pointer to the element. The old
  `for ^item in collection` spelling is gone.
- Formatter architecture is syntax/trivia based and does not require sema.
- Memory ownership uses arenas for phase/request lifetime, dynamic arrays for
  growing tables, and direct heap allocation only for escaped ownership or
  dynamic-array/map backing storage.

Remaining known risks:

- `sema.c` remains a large phase with declaration collection, type checking,
  dependency analysis, and many side-table responsibilities in one unit.
- AST and CST remain separate syntax products by decision, so syntax changes
  still need discipline to avoid parser/tooling drift.
- LLVM lowering is textual and clang-driven. This is the current install
  contract by measurement-backed decision, but bitcode or direct LLVM CLI tools
  remain possible future performance experiments.
- Runtime and ABI choices are deliberately narrow. The accepted target contract
  is the host 64-bit clang target; aggregate FFI, varargs policy expansion,
  debug metadata, target triples, and platform-specific runtime details need
  careful expansion as the language grows.
- LSP semantic readiness is split into declaration, binding, and type-fact
  products, but `sema.c` still builds those facts inside one large pass.
- The formatter has a token/trivia fallback and shared syntax context, but the
  recursive CST path still contains many construct-specific layout rules.

Recommended next review work:

- split guaranteed declaration/binding indexing from full semantic success
- keep centralising AST/CST syntax helpers and revisit unification only when
  duplicated grammar work outweighs the current split
- keep reducing formatter layout cases into trivia/syntax-region rules
- measure clang/textual LLVM startup cost before changing the backend toolchain
- expand the layout context, target datalayout/triple emission, and ABI
  diagnostics before claiming aggregate FFI or cross-target support

## Initial Agreements

- We should not make the syntax AST carry semantic meaning. That would conflict
  with the current roadmap and would make formatter/editor recovery harder.
- A semantically checked intermediate tree is worth investigating, but it should
  be a new layer between semantic analysis and IR rather than a replacement for
  the syntax AST.
- The LSP needs a model that can answer useful questions against incomplete
  source. Treating "compiler succeeds" as the primary gateway for editor
  features will keep producing fragile fallback paths.
- Formatter stability and LSP stability should be reviewed together because both
  need tolerant, source-aware structure.
- Any architectural change should land in small, horizontal increments with
  tests for compiler, formatter, LSP, and diagnostics where behaviour changes.

## Candidate Direction

HIR is now the checked structural intermediate representation between `sema`
and LLVM lowering.

The new layer would be:

- semantically typed and name-resolved
- explicit about scopes, declarations, locals, control flow, and lowered sugar
- still structured enough for source-level tooling and diagnostics
- lower-level than the source AST, but higher-level than target LLVM IR
- independent of parser trivia and formatting concerns

The intended pipeline would become:

```text
lexer -> tolerant CST/syntax parse -> AST -> sema -> HIR -> LLVM IR -> clang
```

Longer term, the editor-facing path could use:

```text
lexer -> tolerant CST/syntax parse -> partial symbol index -> best-effort sema
```

while the build path uses the stricter full pipeline.

## Stage Boundary Review

This review is about whether the current stage boundaries and interfaces are
the right ones, not whether every implementation detail inside each stage is
currently ideal.

### Current Boundaries

The documented and implemented build path has these major products:

- `Lexer`: tokens, source mapping, interned symbols, literal tables, and
  comments.
- `Ast`: compact syntax tree for the compiler.
- `Cst`: source-preserving concrete tree used by formatter and some editor
  tooling.
- `Sema`: declarations, scopes, locals, types, dependencies, constants, generic
  instantiations, and AST-indexed side tables.
- `Hir`: semantically checked lowered program used by LLVM generation and HIR
  snapshots.
- `LLVM`: generated LLVM IR text and backend lowering state.
- `ProgramInfo`: module graph containing each module's `FrontEndState`.

The main aggregate interface is currently:

```c
typedef struct FrontEndState {
    Lexer lexer;
    Ast   ast;
    Sema  sema;
    Hir   hir;
} FrontEndState;
```

This is convenient for the batch compiler, but it hides which products are
actually valid after partial analysis. The LSP already works around this with
extra flags such as `analysis_ok`, `semantic_ready`, and `has_cst`.

### Boundaries That Look Right

- Lexing should remain separate. Tokens, comments, source fragments, line/column
  mapping, and symbol interning are distinct enough to justify a real phase.
- Parsing should remain syntax-only. The roadmap's rule that semantic meaning
  belongs outside the AST is still sound.
- Semantic analysis should own name resolution, type checking, declaration
  classification, dependency ordering, and language-rule diagnostics.
- IR should remain lower than source-level structure and should not become the
  primary editor model.
- Formatting should not depend on semantic analysis.
- LSP should reuse compiler facts where possible, but should not require a full
  successful build to provide basic source-level features.

### Boundary Smells

- `Ast` and `Cst` are separate parsers/products with very similar node families.
  That creates duplication whenever syntax changes and increases the chance that
  formatter/LSP behaviour diverges from compiler parsing.
- `FrontEndState` groups lexer, AST, sema, and IR without phase-validity
  metadata. Callers can observe empty or partial arrays and must infer what is
  safe.
- `Sema` is both the semantic checker and the owner of many downstream facts
  used by LSP and IR generation. Some of those facts may want to be split into a
  stable symbol/type database and a stricter "fully checked" result.
- `IR` still includes semantic type rows and depends on lexer symbols for names.
  This is practical today, but it means the back end still knows about front-end
  representation details.
- Program/module loading is intertwined with front-end products. Module import,
  export collection, per-module sema, and program merging all reach into
  `FrontEndState`.
- The formatter has a clean public API, but internally it owns many layout
  cases directly. This suggests the formatter boundary may be right while the
  layout-rule interface inside it needs design.
- LSP features access `doc->front_end.lexer`, `ast`, and `sema` directly in many
  places. That makes each feature responsible for checking partial-state safety.

### Interface Changes To Consider

#### 1. Split Syntax Products More Deliberately

Possible target:

```text
source -> lexer -> tolerant syntax tree -> compact AST view
```

The tolerant syntax tree would be the source-preserving product for formatter
and LSP. The compact AST could either be derived from it or built by a shared
parser core. The key point is that source syntax should have one authoritative
parse shape, with strict compiler parsing as a validation mode rather than a
parallel grammar.

Questions:

- Should `Ast` be derived from `Cst`, or should both be emitted by one parser?
- Does the current CST preserve enough token/trivia information to become the
  authoritative syntax product?
- How much tolerant/error-node support is required before LSP can stop doing
  repaired-source parses?

#### 2. Make Phase Readiness Explicit

Possible target:

```c
typedef struct FrontEndProducts {
    Lexer lexer;
    Ast ast;
    Sema sema;
    CheckedCore core;
    Ir ir;
    FrontEndReady ready;
} FrontEndProducts;
```

or separate return types for each stage:

```text
LexResult -> ParseResult -> SemaResult -> CoreResult -> IrResult
```

The goal is to stop exposing "maybe valid" arrays as an implicit contract.

Questions:

- Should batch compilation fail fast while LSP receives partial products?
- Should phase results carry diagnostics directly instead of relying on the
  global error render mode?
- Which products are valid after a failed sema pass, and can that be expressed
  mechanically?

#### 3. Split Semantic Facts From Semantic Success

The LSP needs partial symbol, scope, declaration, and sometimes type facts even
when the program is not semantically valid. The compiler needs stricter facts
before lowering.

Possible split:

- `NameIndex`: declarations, lexical scopes, imports, public exports, and simple
  binding/reference relationships.
- `TypeFacts`: best-effort type rows and expected-type information.
- `SemaChecked`: proof that the source satisfies language rules and can lower.

Questions:

- Can declaration collection and scope indexing become a guaranteed product
  after syntax parse?
- Can unresolved or invalid type rows be represented explicitly instead of by
  missing side-table entries?
- Should LSP use public accessor APIs rather than reading `Sema` arrays
  directly?

#### 4. Add A Checked Core Boundary

The proposed checked core should be the first product that assumes semantic
success. It should have no parser trivia and no unresolved source ambiguity.

Possible ownership:

- `sema`: validates source and produces semantic facts.
- `core`: lowers checked source structure into a smaller structured language.
- `llvm`: lowers checked HIR into target LLVM IR.

Questions:

- Does checked core own sugar lowering, or does sema own some lowering during
  checking?
- Should checked core be expression/statement structured, control-flow graph
  shaped, or close to C blocks/statements?
- What is the first construct to move through this boundary as a prototype?

#### 5. Narrow The Back-End Interface

The back end should ideally consume HIR plus a small target/layout context,
rather than a broad `FrontEndState`. The current LLVM path still needs lexer
and sema context for stable text rendering and type/layout queries, but it no
longer depends on the retired IR or C generation tables.

Questions:

- Can `back_end()` take HIR and a symbol/type/layout context instead of
  `FrontEndState`?
- Can program merging happen before or during HIR generation rather than in the
  executable back end?
- Which lexer and sema dependencies in LLVM generation are truly rendering or
  layout needs?

#### 5a. Historical: HIR-To-C Without A Separate IR

This was an intermediate option before LLVM became the executable backend. It
is preserved as context for why HIR absorbed the services that had previously
belonged to the custom IR.

The retired IR provided real services that HIR or target lowering still needed
to own:

- declaration/function/global tables
- explicit locals and temporaries
- stable lowered values with types
- lowered calls, casts, field/index operations, and aggregate construction
- explicit string-builder sequencing
- dynamic-array operation records
- control-flow labels, jumps, and branches
- generated init function structure
- module merge and symbol remapping support
- stable IR rendering/dumping for tests and debugging

The outcome was HIR-to-LLVM rather than HIR-to-C. HIR still should not become
"IR with braces"; the useful version remains a structured, typed core language
with:

- explicit declaration records
- typed expression and statement nodes
- explicit block/function ownership
- lowered sugar where source syntax is too rich
- a clear temporary introduction policy
- LLVM lowering that walks structured functions and statements

Questions:

- Do we still want a backend-neutral representation for a future VM or non-C
  backend?
- Did the retired IR preserve semantics that should be explicit in HIR?
- Can structured HIR represent `defer`, value-producing loops, `on`, and pattern
  matching without immediately lowering to labels and jumps?

#### 5b. Completed: Dropping The Current IR

The old IR was removed once HIR became the checked, typed, lowered program
product. The important observation from the code review was that the old IR was
not primarily an optimizer IR. It was a linear C-generation input plus several
side tables:

- program structure: globals, externs, functions, locals, params
- typed values and temporaries
- lowered calls, casts, aggregate construction, fields, indexes, and slices
- dynamic-array and string-builder operation records
- explicit labels, jumps, branches, init blocks, and returns
- copied semantic type rows used by the old C generation path
- module merge/remapping support for symbols, types, strings, functions, and
  init ordering

Dropping IR was possible because HIR and LLVM lowering now own these services
deliberately. The target is not "AST directly to a backend"; it is:

```text
lexer -> tolerant syntax -> sema/name/type facts -> checked HIR -> LLVM IR
```

In that model HIR is the durable lowering boundary. The old IR disappeared once
HIR could represent:

- explicit function/global/extern records
- explicit locals, params, and compiler temporaries
- structured blocks plus enough lowered control flow for `on`, loops, `defer`,
  and expression-valued control constructs
- typed expression nodes for calls, casts, field/index operations, slices,
  aggregates, string interpolation, and dynamic-array operations
- module-level init sequencing
- module merge or whole-program symbol/type identity

#### 5c. Completed: LLVM IR As The Backend Target

LLVM IR removed C-specific pressure from the pipeline, especially around
declaration order and generated C spelling. The compiler still has ordering
paths with different motivations:

- semantic declaration dependency ordering in `sema` for inference, constant
  evaluation, and cycle diagnostics
- module load order and init sequencing in the back end
- generated LLVM names and aliases during whole-program lowering

LLVM IR would not remove semantic dependency analysis. The language still needs
cycle diagnostics, type inference order, constant evaluation order, and module
import validation. But it could remove or simplify C-specific ordering work:

- functions can be emitted with generated internal names and referenced before
  their textual definition
- Nerd binding names can become aliases, metadata, or debug names instead of
  direct backend symbol names
- nominal runtime/type names can be generated from stable ids rather than from
  C-safe spelling and topological declaration order
- mutually referential functions and many aggregate references are less awkward
  than they were in generated C

Candidate naming model:

```llvm
@$main = ...                ; Nerd-visible function binding
@fn.42 = internal ...       ; compiler-generated helper or lowered function
```

Nerd-visible bindings keep the `$` prefix followed by the Nerd binding name.
For example, `main :: fn () { ... }` becomes `@$main` in LLVM IR. Opaque
generated ids such as `@fn.N`, `@global.N`, and `%type.N` are still useful for
compiler-created helpers, lowered lambdas, temporary globals, or internal
runtime glue that has no direct Nerd binding. The original source name should
also be available in debug metadata.

Tradeoff:

- C currently gives the backend a large prelude/postlude for free: headers,
  typedef spelling, platform C ABI integration, libc declarations, linking
  conventions, startup shape, and runtime helper declarations.
- LLVM would require us to own the equivalent: target triples/data layout,
  linkage/visibility, function attributes, debug metadata, ABI details for
  aggregates and varargs, runtime helper declarations, platform library
  linkage, and the module entry/postlude shape.

Current runtime bridge:

- The generated LLVM wrapper names `$main()`, which matches the intended LLVM
  symbol convention for a Nerd `main` binding.
- The runtime object comes from `data/nrt.c`, so the LLVM link step still needs
  the platform C runtime and correct target flags.
- Runtime helper names such as `string_eq`, `string_slice`,
  `string_builder_*`, and `to_string$*` need a stable LLVM naming/linkage
  contract.
- The current build cache is based on `//> run` directive outputs and
  dependency mtimes. If runtime artifacts become target-specific, the cache key
  must also include target triple, release/debug mode, and relevant compiler
  flags.

#### 6. Define LSP Feature Contracts

Each LSP feature should have a declared minimum product:

- semantic tokens: tokens plus optional semantic colouring facts
- document symbols: tolerant syntax/declaration index
- completion: declaration index plus optional type facts
- hover: token plus declaration/type facts when available
- definition: binding/reference index
- rename: binding/reference index and source ranges
- code actions: syntax plus targeted semantic facts where needed

Questions:

- Which features should work with only tokens and syntax?
- Which features should degrade gracefully when type facts are missing?
- Should LSP operate through an `AnalysisSnapshot` API instead of direct access
  to compiler internals?

## Why This May Help

### Compiler

A checked core can absorb sugar-lowering and normalisation currently split
between semantic analysis and IR generation. Examples that may belong there:

- default argument substitution
- desugaring expression blocks and value-producing loops
- normalising pattern matching into simpler control flow
- making method calls explicit
- making generic instantiations explicit
- resolving field/index operations to checked forms

That would let IR generation consume one simpler language instead of repeatedly
interpreting source syntax plus semantic side tables.

### LSP

The LSP should be able to answer:

- what symbol is visible here?
- what declaration owns this token?
- what type is expected here?
- what fields/methods are available after this receiver?
- where is this binding declared?

Those questions do not always need a complete compile-ready program. A layered
model could provide:

- lexical tokens and trivia for basic editor features
- tolerant CST for local structure, ranges, and formatter support
- partial declaration/scope index for completions and same-file navigation
- best-effort semantic facts where type checking succeeds
- checked core only when source is valid enough for stricter lowering

This should reduce repaired-source parsing and one-off fallback code in LSP
features.

### Formatter

The formatter should probably move further toward token/CST rewriting with a
small set of declarative layout rules. A checked core is not the formatter's
input, but a better tolerant CST would help the formatter recover from edge
cases and keep layout decisions source-structural instead of semantic.

## Formatter Architecture Option

The formatter may work better as a token/trivia-driven layout engine rather
than as a recursive CST pretty-printer.

Current formatter shape:

- lexes in `LEXER_MODE_FORMAT` so comments are captured
- scans source line-by-line to handle comment paragraphs and blank lines
- extracts code blocks
- reparses those blocks with CST
- emits formatted text from CST nodes with many construct-specific cases

This already uses the lexer and CST, but the output driver is split between
source-line scanning and recursive CST emission. That split explains some edge
cases: comments, blank lines, original newlines, and syntax structure are not
all represented in one stream.

### Token/Trivia State Machine

Candidate model:

```text
source -> lexer tokens + comments + newline trivia
       -> syntax/sema context tables
       -> formatter state machine
       -> output tokens with spacing/newlines/indent decisions
```

The formatter would walk a canonical token stream. Before emitting each token,
it would decide:

- whether to insert a blank line
- whether to insert a newline
- whether to indent or dedent
- whether to insert a space
- whether to attach, align, or reflow comments
- whether a construct should force multiline layout

The formatter state would include:

- current token index
- previous emitted token kind
- current line/column
- current indentation level
- delimiter depth for `()`, `[]`, and `{}` groups
- a scope/layout stack
- pending comments/trivia before the current token
- current declaration/statement/expression context from syntax tables
- optional semantic facts for layout choices that need resolved meaning

Possible layout stack entries:

- file
- declaration group
- function signature
- parameter list
- block
- plex/union/enum field group
- plex literal field group
- `on` branch group
- loop header
- call argument list
- type argument list
- expression continuation

Each token would be emitted by a small rule like:

```text
before(token, context, stack) -> layout actions
emit(token)
after(token, context, stack) -> stack updates
```

This is closer to a formatter VM than to a tree printer.

### Role Of Syntax And Sema

The token stream should drive output, but syntax still matters. A formatter
cannot reliably decide whether `{` starts a block, a plex type, a plex literal,
or an `on` branch from token kind alone.

Recommended split:

- lexer/trivia stream owns exact token order, comments, and original line
  boundaries
- CST or tolerant syntax owns token-to-node and node-to-construct context
- sema is optional and should be used only where syntax is genuinely ambiguous
  after parsing

Sema should not be required for ordinary formatting. Requiring sema would make
formatting invalid or partial source much weaker, and it would make formatting
depend on imports and type correctness. However, sema could be useful for
optional future formatting choices such as distinguishing type applications from
indexing if the syntax layer cannot classify them.

### Trivia Requirements

The lexer currently skips whitespace tokens and records comments only in format
mode. A token-state formatter would need a more explicit trivia model:

- newline count before each token
- whether blank lines existed before each token
- comments before each token
- end-of-line comments after a token
- original indentation for comment-only lines
- whether comments are standalone, trailing, or continuation comments

This does not necessarily require whitespace tokens. Dense side tables keyed by
token index would probably fit the codebase better.

Possible product:

```c
typedef struct FormatTrivia {
    Array(u16) newlines_before_token;
    Array(u32) first_comment_before_token;
    Array(u16) comment_count_before_token;
    Array(u32) trailing_comment_index_by_token;
} FormatTrivia;
```

### Benefits

- One output path for tokens, comments, and newlines.
- Fewer recursive special cases that manually remember comment attachment.
- Better idempotence because layout decisions happen at consistent token
  boundaries.
- Easier tolerant formatting because the token stream exists even when full CST
  parsing fails.
- Scope stack gives a natural home for indentation and multiline decisions.
- Existing alignment rules become local passes over token ranges or layout
  stack regions rather than ad hoc emission branches.

### Risks

- A token-only formatter can become a maze of local exceptions unless syntax
  context is available cheaply.
- Formatting constructs with generated alignment still needs region-level
  planning before token emission.
- Reflowing comments is paragraph-oriented, not token-oriented, so comment
  handling still needs its own small subsystem.
- If sema is required, formatting becomes too fragile for broken code and
  incomplete editor buffers.
- The formatter may need a prepass that classifies token ranges into formatting
  regions before the state machine emits output.

### Possible Migration Path

1. Add explicit newline/comment trivia tables from the existing format-mode
   lexer.
2. Add token-to-CST-node or token-range context tables.
3. Implement a token-stream formatter for one narrow construct, such as use
   declarations or local binding paragraphs.
4. Move comment attachment and blank-line preservation into trivia-driven rules.
5. Convert alignment groups to region prepasses.
6. Keep recursive CST emission for constructs not yet migrated.
7. Remove old emission paths only after formatter idempotence tests cover the
   migrated regions.

### Open Questions

- Should newline/comment trivia live in `Lexer`, a `FormatTrivia` product, or a
  future tolerant CST?
- Should the formatter be able to run with only tokens and trivia when parsing
  fails?
- Which formatting decisions truly require sema, if any?
- Can a scope stack replace most current construct-specific indentation code?
- Should alignment be computed as a prepass over token ranges before emission?

## LSP Stability Notes

Current LSP analysis is whole-document and intentionally keeps partial front-end
results. That is a reasonable starting point, but it creates sharp edges:

- parse failure can prevent semantic data from existing at all
- semantic failure can leave only some side tables populated
- feature files must know which side tables are safe after each failure mode
- completion and hover carry feature-specific fallback paths
- crashes are likely where a feature assumes a table row exists because normal
  compilation would have produced it

Architectural target:

- define explicit readiness levels for document state
- make each LSP feature declare the minimum readiness level it needs
- add checked accessors for semantic side tables so missing rows produce no
  result instead of an out-of-bounds access
- keep source, tokens, CST, partial declarations, and semantic facts as separate
  products with separate validity flags

Possible readiness levels:

1. `source_ready`: buffer text is stored.
2. `tokens_ready`: lexing produced token ranges.
3. `syntax_ready`: tolerant CST exists, possibly with error nodes.
4. `decls_ready`: top-level and local binding candidates are indexed.
5. `sema_partial`: some names/types resolved with explicit validity bits.
6. `sema_complete`: full semantic analysis succeeded.
7. `core_ready`: checked core exists and can lower to IR.

Detailed evidence is recorded in `review/audits/lsp-crashes.md` and
`review/audits/lsp-boundaries.md`.

Migration status:

- LSP document readiness is now exposed through source, token, syntax, and
  semantic views.
- Feature handlers no longer look up documents directly from the document map;
  direct lookup is contained in document/view construction and lifecycle code.
- LSP feature code now uses checked accessors for semantic declaration, local,
  type, and AST-indexed side-table reads.
- Imported module access now has a checked module view and export-declaration
  accessor used by completion, hover/definition, and code actions.
- The remaining architectural question is whether `sema_partial` should split
  into declaration, binding, type, and imported-module readiness products.

## Memory Strategy Review

Memory strategy should be part of the architecture discussion because the
compiler and LSP are allocation-heavy. The key performance questions are:

- how many allocations happen per source edit or build?
- how often does data move because a dynamic array grows?
- how cache-friendly are compiler passes over their main data products?
- how clear is ownership at API boundaries?

### Current Model

The C implementation already uses three main strategies:

- OS-backed arenas in `src/core/arena.c`
  - reserve a large virtual range
  - commit pages as the cursor grows
  - free/reset the whole region at once
  - support store/restore marks and arena sessions
- Heap-backed dynamic arrays in `src/core/array.c`
  - `Array(T)` is a pointer with a header
  - starts at capacity 4
  - grows by doubling through `realloc`
  - stores elements contiguously
- Heap allocation wrappers in `src/core/memory.c`
  - add debug leak tracking and allocation metadata
  - used by arrays, maps, shell/path helpers, and individually owned values

The language standard library mirrors this split:

- `std.mem` wraps raw C allocation.
- `std.arena` provides heap and mmap-style arena helpers.
- dynamic arrays own growable storage and require `.free()`.
- slices borrow storage and do not own memory.

### Performance Characteristics

#### Arenas

Arenas are best when many objects share one lifetime.

Good fits:

- per-command compiler outputs
- per-module or per-program analysis products
- LSP per-document snapshots
- diagnostic/rendering scratch
- generated text buffers
- short-lived temporary formatting/completion buffers
- strings or records that never need individual deletion

Performance strengths:

- near-zero per-allocation overhead after page commit
- excellent cleanup cost: reset or done once
- stable pointers because the arena base does not move
- good locality when related data is allocated in pass order

Costs and risks:

- memory is not reclaimed per object
- accidental long-lived arenas can retain too much data
- default reserve sizes can look large even when physical commit is small
- store/restore discipline is required for temporary use
- not suitable for data that needs frequent deletion or resizing in place

Guidance:

- Use arenas for phase products with a clear owner and clear destruction point.
- Prefer one arena per analysis snapshot or major product over many small
  arenas created inside hot loops.
- Use arena marks for temporary scratch inside a pass when the scratch does not
  escape.
- Avoid allocating dynamic-array backing stores from arenas unless the array is
  append-only or its maximum size is known.

#### Dynamic Arrays

Dynamic arrays are best for ordered collections whose count is not known
upfront and whose contents need indexable contiguous storage.

Good fits:

- tokens
- AST/CST node rows
- semantic side tables
- IR/HIR rows
- diagnostic arrays
- worklists
- LSP result lists

Performance strengths:

- contiguous storage is cache-friendly
- simple append and indexed traversal
- growth is amortised
- easy to pass around as `T*` plus hidden count/capacity metadata

Costs and risks:

- heap allocation on first use
- repeated growth copies data
- pointer can move on growth
- deletion from the middle costs `memmove`
- many tiny arrays produce many heap allocations

Guidance:

- Prefer dense dynamic arrays over linked lists or per-node heap objects.
- Reserve capacity when a good upper bound is known, especially for token,
  node, and side-table arrays.
- Prefer parallel arrays for dense compiler facts when they are traversed
  together by index.
- Prefer swap-delete for unordered worklists and preserve-order delete only
  where ordering matters.
- Do not store pointers into dynamic arrays across calls that may grow them;
  store indices instead.

#### Heap Allocations

Individual heap allocations are best when an object has independent ownership
or must outlive a phase/snapshot.

Good fits:

- file maps and OS resources
- map backing storage
- data with explicit independent lifetime
- user/runtime dynamic arrays
- large buffers that need manual release independent of compiler phase cleanup

Performance strengths:

- independent lifetime
- can be resized or freed independently
- debug allocator gives useful leak tracking

Costs and risks:

- highest per-allocation overhead
- debug mode maintains allocation metadata and linked-list bookkeeping
- poorer locality when many small objects are allocated separately
- ownership must be explicit and tested

Guidance:

- Avoid per-node heap allocation in compiler data structures.
- Use heap allocation for ownership boundaries, not as the default container
  strategy.
- Keep heap-owned APIs explicit about who frees and when.

### Compiler Data Recommendations

- Lexer
  - Tokens and literal tables should remain dense arrays.
  - Comment text and decoded strings fit arenas because they share lexer
    lifetime.
  - Consider reserving token/comment capacity from source size heuristics to
    reduce reallocations.
- Syntax trees
  - AST/CST/HIR nodes should remain dense, index-addressed rows.
  - Avoid pointer-heavy node graphs.
  - If CST becomes tolerant and authoritative, keep trivia/ranges in dense side
    tables rather than per-node allocations.
- Semantic analysis
  - Declaration, local, scope, type, and node side tables should remain dense
    arrays.
  - Best-effort/partial semantic facts should use validity bits or sentinel
    rows rather than sparse heap objects.
  - Per-pass worklists should be dynamic arrays, ideally pre-sized from node or
    declaration counts.
- HIR/Core
  - Prefer one arena-owned product with dense row arrays.
  - Store references as indices to declarations, locals, types, blocks, and
    expressions.
  - HIR now owns the middle-layer facts that used to make the old IR necessary.
- LLVM generation
  - Generated text should remain arena/string-builder based.
  - LLVM generation should avoid many small temporary arenas inside tight
    loops; prefer one scratch arena with marks when possible.
- Module/program state
  - Program-level arenas are a good fit for module graph strings and stable
    metadata.
  - Per-module products should be independently releasable for LSP snapshots and
    future incremental compilation.

### LSP Memory Recommendations

LSP memory has a different profile from batch compilation because analysis is
repeated on every edit.

- Keep separate arenas for source text and analysis products, as today.
- Treat each analysis result as a snapshot with one owner and one cleanup point.
- Reuse arenas across edits with `arena_reset` only when no old snapshot can be
  observed by an in-flight request.
- If future async or parallel LSP work is added, use immutable snapshots and
  retire whole snapshots rather than mutating shared analysis in place.
- Avoid many short-lived `arena_init`/`arena_done` pairs in hot completion paths;
  prefer request-level scratch arenas with marks.
- Cache module exports and stable standard-library summaries where correctness
  permits, but make invalidation explicit.

### Language And Standard Library Questions

The user-facing language should make allocation choices visible where they
matter for performance and ownership.

Questions:

- Should more standard-library APIs accept an arena/allocator parameter for
  temporary output, instead of always returning dynamic arrays?
- Should dynamic arrays support explicit initial capacity at construction sites
  beyond the current `[N..]T` form?
- Should there be a standard "borrowed builder" or arena-backed builder type for
  strings and byte buffers?
- Should functions that allocate be named or typed in a way that makes ownership
  obvious?
- Should the compiler/linter warn when a dynamic array is probably leaked, or is
  `defer array.free()` documentation enough for now?

### Initial Policy Proposal

- Use arenas for group lifetime.
- Use dynamic arrays for mutable contiguous collections.
- Use heap allocations for independent ownership and OS/resource boundaries.
- Use slices for borrowed views.
- Use indices, not pointers, across growable compiler products.
- Reserve capacity when a phase has a cheap size estimate.
- Make allocation ownership part of public API documentation.
- Measure before replacing dynamic arrays with custom arena arrays; the major
  win may be fewer growth reallocations, not arenas by themselves.

## Language Design Questions

These are places where language changes may simplify the compiler and LSP:

- Can ambiguous bracket syntax be reduced, or is semantic classification of
  index/slice/generic application an accepted permanent cost?
- Should there be clearer grammar boundaries between type expressions and value
  expressions to reduce parser and completion ambiguity?
- Are expression blocks, labelled value-producing loops, and `on` expressions
  pulling too much complexity into every downstream stage?
- Should named/default arguments remain a surface sugar that always lowers
  before checked core?
- Should module import/export rules expose a stable symbol-index API for LSP
  rather than making completion re-read imported source files?
- Can destructuring and pattern binders be represented as simple generated
  locals in checked core?

No decision is recorded here yet. These need examples and cost estimates before
changing the language.

## Proposed Review Workstreams

1. LSP crash audit
   - Reproduce the current crashes.
   - Classify each crash by missing product: tokens, syntax, decls, sema row,
     module state, or invalid range.
   - Add guard tests before refactoring.

2. Formatter edge-case audit
   - List the unstable formatter cases.
   - Separate token/trivia issues from CST-shape issues and layout-rule issues.
   - Add idempotence tests for each accepted edge case.

3. Checked core sketch
   - Define the minimal node/row model.
   - Choose what sugar lowers into core versus what remains for IR generation.
   - Prototype one narrow construct before migrating broad compiler behaviour.

4. Partial analysis model
   - Define document readiness levels.
   - Add safe semantic accessors for LSP.
   - Convert one LSP feature, probably hover or definition, before completion.

5. Language simplification review
   - Pick concrete syntax features that cause repeated downstream complexity.
   - For each, compare keeping the feature, changing it, or lowering it earlier.
   - Record source compatibility impact before deciding.

## Repository Organisation

Architecture review material should be durable, easy to diff, and close enough
to the repo that decisions can be reviewed with code changes. It should not
turn into a second documentation tree or a pile of one-off scripts.

Recommended layout:

```text
ARCHITECTURE_REVIEW.md
review/
  README.md
  decisions/
  audits/
  prototypes/
  measurements/
```

### Root Review Document

Keep `ARCHITECTURE_REVIEW.md` as the top-level index and current synthesis:

- current agreements
- open questions
- active workstreams
- links to detailed audits and decisions
- latest recommendation

This file should stay readable as the "where are we?" document. Detailed raw
notes should move under `review/`.

### `review/decisions`

Use short decision records when we settle something.

Suggested filename:

```text
review/decisions/0001-checked-core-direction.md
```

Suggested shape:

```text
# 0001: Title

Status: proposed | accepted | rejected | superseded
Date: YYYY-MM-DD

## Context
## Decision
## Consequences
## Follow-up
```

Decision records should be for real agreements, not every thought.

### `review/audits`

Use audits for evidence-gathering before a decision.

Examples:

```text
review/audits/lsp-crashes.md
review/audits/formatter-edge-cases.md
review/audits/sema-ir-responsibilities.md
review/audits/memory-allocation-profile.md
```

Audits should include:

- reproduction steps or commands
- files/functions involved
- observed behaviour
- classification
- proposed next action

### `review/prototypes`

Use this for prototype notes, not production prototype code.

Example:

```text
review/prototypes/token-trivia-formatter.md
review/prototypes/hir-expression-block.md
```

If a prototype needs code, prefer putting the code in the normal source tree
behind a narrow command/test path rather than keeping long-lived code under
`review/`.

### `review/measurements`

Use this for captured benchmark or profiling results.

Examples:

```text
review/measurements/2026-05-07-format-allocation-baseline.md
review/measurements/2026-05-07-lsp-completion-baseline.md
```

Measurement notes should include:

- commit or working-tree context
- command used
- machine/OS details when relevant
- raw result summary
- interpretation

### Scripts

Do not add a separate Python script just to organise the review. Add scripts
only when the task is repeatable and easy to get wrong manually.

Good candidates for scripts:

- run formatter idempotence cases and summarise failures
- replay LSP crash/completion traces
- count allocations or dynamic-array growth after instrumentation exists
- generate a small report from `just test` timing output

Recommended script location:

```text
build/review_<name>.py
```

or, if several review scripts accumulate:

```text
build/review/<name>.py
```

Add `just` recipes only for scripts that should become normal workflow:

```text
just review-format
just review-lsp
just review-memory
```

### Initial Setup Proposal

Start with docs only:

```text
review/README.md
review/audits/lsp-crashes.md
review/audits/formatter-edge-cases.md
review/audits/memory-allocation-profile.md
review/prototypes/token-trivia-formatter.md
```

Then add scripts after the first audit reveals a repeated command worth
automating.

## Near-Term Recommendations

- Do not start by rewriting the compiler pipeline.
- Start with LSP crash reproduction because crashes reveal invalid assumptions
  about partial compiler state.
- In parallel, document formatter edge cases as tests before changing format
  architecture.
- Prototype a checked core only after one or two concrete lowering targets are
  chosen. Good candidates are default arguments, method calls, or expression
  block lowering because they are visible enough to prove the layer's value
  without owning the whole language.

## Decision Log

No dated decisions yet.
