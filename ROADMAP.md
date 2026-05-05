# Roadmap

This document records current project guidance and active work. Completed
milestone plans should live in git history, tests, and documentation rather than
continuing to grow this file.

## Current State

- The compiler pipeline is `lexer -> CST/AST parser -> sema -> IR -> C -> clang`.
- Semantic analysis is a distinct front-end phase and owns name resolution,
  declaration classification, typing, diagnostics, and dependency ordering.
- The AST should remain compact and syntax-oriented. Semantic facts belong in
  arena-backed side tables keyed by indices.
- The CST owns source-preserving tooling such as formatting and editor-facing
  token structure.
- `just test` is the full-project regression gate. It runs language, error,
  formatter, LSP, and command tests.
- Modules load through the program-level front end. Standard modules live under
  `mods/` and should be treated like normal source modules.
- Module paths resolve to `path.n` first, then `path/mod.n` for folder modules.
- Folder modules implicitly include immediate sibling `.n` files in the same
  module scope.
- Public module exports are explicit with `pub`. Non-exported module members
  are private.

## Working Rules

- Do the work in small, reviewable increments.
- Prefer stable intermediate representations over ad hoc cross-stage logic.
- Keep new compiler data structures arena-based, table-based, and index-based.
  Prefer dense parallel tables and side tables over pointer-heavy object graphs.
- Avoid expanding `AstNode` unless there is a strong measured reason.
- Compute spans from token and node boundaries rather than storing large span
  payloads in AST nodes.
- Preserve the AST's RPN-friendly structure so it remains suitable for
  stack/VM-style passes, constant folding, and later compile-time execution.
- Keep parser responsibilities syntactic. Put symbol meaning, declaration
  meaning, type resolution, and language rules in semantic analysis.
- Use British spelling throughout code, comments, tests, documentation, and git
  commit messages where practical.
- In generated C, emit a leading `$` prefix only for C symbols that correspond
  directly to Nerd-language names. Keep hidden runtime/compiler helper names
  such as `init` unprefixed.
- Allow forward references between declarations where the language already
  supports them. Track dependencies explicitly and order declarations from that
  information.
- Keep top-level constant bindings simple: lower them as globals/constants and
  let the C compiler handle optimisation unless a specific compiler optimisation
  has been designed.
- When touching a compiler file, improve nearby comments and spelling where it
  is useful, but do not turn local fixes into unrelated rewrites.

## Horizontal Feature Policy

New language features should land across the toolchain together:

- compiler parsing and semantic analysis
- IR and C generation
- formatter support
- LSP/editor support where relevant
- language tests
- error tests for invalid source
- format/LSP/command tests where the feature affects those surfaces
- user-facing docs when the language surface changes, including the manual and
  syntax/language-reference appendices where relevant

Prefer dense language regression tests that exercise related runtime cases in
one file. Use `std.io` for visible runtime output rather than adding many tiny
happy-path tests.

Do not move on from a language feature while one of the agreed compiler,
formatter, LSP, testing, or manual/documentation surfaces is knowingly behind.
If a documentation update is intentionally deferred, record that explicitly in
the roadmap before committing the implementation.

## Diagnostic Policy

- Keep compiler error-code ranges phase-specific:
  - `0100`-`0199` lexer
  - `0200`-`0299` parser / AST construction
  - `0300`-`0399` semantic analysis
- Keep the fuller diagnostic design policy in
  [docs/error-system.md](docs/error-system.md), and update that document when
  error-shaping rules change.
- Every language-compilation failure point must produce either:
  - a categorised compiler diagnostic with source spans, useful references, and
    appropriate notes or help text, or
  - an ICE when the failure represents a compiler bug or violated internal
    invariant rather than invalid user source.
- Use notes for explanatory context about why an error happened or which rule
  applies.
- Use help for actionable fix guidance.
- Keep `nerd explain <code>` category-level rather than case-level, so error
  codes remain broad and stable.
- Treat OS, filesystem, shell, and toolchain failures as runtime errors, not
  user-source diagnostics or ICEs.
- Treat IR generation and C generation invariant failures as ICEs.

## Test Artefact Policy

- Passing tests should remove generated intermediate files.
- Failing tests should keep generated intermediate files so failures can be
  analysed locally.
- When a feature changes CLI-visible compile-time behaviour, add at least one
  regression that exercises the real `nerd` command path rather than only
  internal compiler entry points.

## Parser And Sema Direction

- Replace scattered token classification logic with small shared tables or
  macros when doing so reduces local complexity.
- Use compact AST/sema emit helpers where they remove repetitive parser or
  semantic boilerplate.
- Keep semantic analysis VM-friendly and table-driven:
  - process AST/CST data through linear or index-based passes where practical
  - store semantic data in side tables keyed by AST node, symbol, declaration,
    local, and scope indices
  - keep dependency data explicit
- Standardise parser helper contracts:
  - each helper should clearly either require the first token to have already
    been consumed, or consume it itself
  - comments and function names should make the contract clear where ambiguity
    is likely
- Standardise function comments as files are edited:
  - one short purpose comment when it helps
  - separator comments before major internal functions where the file already
    uses that style

## Active Work

### Standard Library Expansion

- Continue expanding the standard library only as the module/export model needs
  real validation.
- Keep the standard library surface disciplined while the core language is still
  moving.
- Prefer simple modules that exercise existing language features before adding
  library-driven language changes.
- Keep `docs/stdlib.md` as a separate standard-library document; the language
  manual should reference the standard library only for small examples.
- [ ] Add `Option[T]` and `Result[T, E]` to the standard library.
- [ ] Keep parsing traits such as `Parse` at standard-library level rather than
  making them language-known traits.

### Function Enhancements Milestone

- [x] Add default parameters with syntax `name: Type = expr`.
- [x] Keep first-version call semantics trailing-only:
  - [x] defaulted parameters must follow non-defaulted parameters
  - [x] calls may omit only trailing defaulted arguments
  - [x] explicit arguments override defaults
- [x] Evaluate defaults at the call site.
- [x] Type-check each default expression against its parameter type.
- [x] Allow a default expression to reference earlier parameters.
- [x] Reject a default expression that references later parameters.
- [x] Reject default parameters on FFI declarations.
- [x] Keep defaults out of function types initially. Defaults belong to known
  function declarations, not to the callable type.
- [x] Require full arity when calling through a function-typed value:
  - [x] allow omitted arguments only when the callee resolves to a known
    function declaration with defaults
  - [x] reject omitted arguments when the callee is an arbitrary function value
- [x] Parser and AST work:
  - [x] extend parameter parsing to accept optional default expressions
  - [x] store the default expression node index on `AstParam`
- [x] Sema work:
  - [x] validate default-parameter ordering
  - [x] resolve and type-check defaults in a parameter/default scope
  - [x] enforce earlier-parameter-only references
  - [x] fill omitted trailing arguments during call checking for known function
    declarations
- [x] IR/C generation work:
  - [x] lower substituted default expressions without adding new runtime
    calling-convention machinery
  - [x] keep generated function signatures unchanged
- [x] Formatter work:
  - [x] format default parameters as `name: Type = value`
  - [x] preserve readable wrapping for long signatures with defaults
- [x] LSP/editor work:
  - [x] show defaults in hover-rendered function signatures
  - [x] record signature-help display as future editor work because signature
    help does not exist yet
  - [x] add/update semantic token coverage if default expressions expose gaps
- [x] Tests:
  - [x] language tests for omitted trailing defaults
  - [x] language tests for explicit override
  - [x] language tests for defaults referencing earlier parameters
  - [x] error tests for required parameters after defaulted parameters
  - [x] error tests for bad default expression type
  - [x] error tests for later-parameter references
  - [x] error tests for defaults on FFI declarations
  - [x] error tests for omitted arguments through function-typed values
  - [x] format tests for default-parameter spacing and wrapping
  - [x] LSP tests for hover/signature text affected by defaults
- [x] Documentation:
  - [x] manual section for default parameters and call-site evaluation
  - [x] syntax-reference appendix entries
  - [x] language-reference appendix rules

### Generics Milestone

- [x] Add generic syntax using square brackets:
  - [x] generic functions as `fn [T] (...) -> R`
  - [x] generic type declarations as `plex [T]`, `union [T]`, and `enum [T]`
  - [x] generic type application as `Name[T]`
  - [x] explicit generic function calls as `name[T](...)`
  - [x] concrete generic function values as `f := name[T]`
- [x] Keep the first generics version type-parameter-only:
  - [x] no constraints until the traits milestone
  - [x] no numeric/value generic parameters in this milestone
  - [x] no partial generic application
- [x] Enforce all-or-nothing generic argument rules:
  - [x] calls may use full inference with no explicit generic arguments
  - [x] calls may use a complete explicit generic argument list
  - [x] reject partially explicit generic argument lists
  - [x] require all generic type arguments for standalone concrete function
    values such as `id[i32]`
- [x] Add type inference for generic function calls:
  - [x] infer type parameters from call arguments
  - [x] type-check explicit generic calls against supplied type arguments
  - [x] produce clear diagnostics when inference cannot determine a type
    parameter
  - [x] include `help` text for missing or non-inferable generic arguments,
    explaining when to omit the whole generic argument list or provide every
    type explicitly
- [x] Add parser and AST support:
  - [x] parse generic parameter lists on function and type declarations
  - [x] parse bracket application syntax without assuming whether it is an
    index/slice or generic application
  - [x] keep ambiguous bracket syntax syntactic in the AST where practical
- [x] Add semantic bracket classification:
  - [x] classify bracket syntax as value indexing/slicing or generic
    application during semantic analysis
  - [x] resolve explicit generic arguments as types
- [x] Add generic type support:
  - [x] instantiate generic plex types
  - [x] instantiate generic union types
  - [x] instantiate generic enum types
  - [x] allow generic type aliases if they fit the same representation cleanly
- [x] Add generic function support:
  - [x] monomorphise generic functions per canonical concrete type argument
    list
  - [x] support direct generic calls
  - [x] support inferred generic calls
  - [x] support concrete instantiated function values such as `id[i32]`
  - [x] force emission of an instantiated function when it is used as a value
- [x] Add instantiation identity and C name mangling:
  - [x] key instantiations by canonical semantic type arguments
  - [x] canonicalise transparent type aliases so equivalent instantiations
    share one lowered body
  - [x] generate C symbols with a readable stem plus a stable hash of canonical
    generic arguments
- [x] IR/C generation work:
  - [x] lower monomorphised functions and types as concrete IR/C entities
  - [x] lower generic types as concrete C entities
  - [x] keep generic type templates out of generated C output
  - [x] preserve stable generated C across rebuilds
- [x] Formatter work:
  - [x] format generic parameter lists
  - [x] format generic type applications
  - [x] format explicit generic calls and concrete generic function values
- [x] LSP/editor work:
  - [x] show generic signatures in hover text
  - [x] include generic parameters in document symbols where useful
  - [x] add semantic-token coverage for generic parameter declarations and uses
- [x] Tests:
  - [x] language tests for generic function inference
  - [x] language tests for explicit generic function calls
  - [x] language tests for concrete generic function values
  - [x] language tests for generic plex/union/enum instantiation
  - [x] error tests for partial explicit generic arguments
  - [x] error tests for inference failure
  - [x] error tests for parsed generic function syntax before semantic
    instantiation
  - [x] format tests for generic declarations and applications
  - [x] LSP tests for hover/symbol/token behaviour affected by generics
- [x] Documentation:
  - [x] manual section for generics
  - [x] syntax-reference appendix entries
  - [x] language-reference appendix rules
  - [x] note that constraints are deferred to a future traits milestone

### Inherent Impl Methods Milestone

- [x] Add inherent impl syntax for compound types:
  - [x] parse `impl Type { ... }`
  - [x] parse generic impl targets such as `impl Stack[T] { ... }`
  - [x] allow `pub` on method bindings inside an impl block
  - [x] reject non-function bindings inside impl blocks
- [x] Add method resolution:
  - [x] resolve `value.method(args...)` to a matching inherent method
  - [x] infer generic impl parameters from the receiver type
  - [x] require all generic method parameters to be inferred
  - [x] keep dynamic-array built-in methods working
  - [x] import public methods across modules
- [x] Add lowering:
  - [x] lower method calls as ordinary function calls with the receiver inserted
  - [x] pass value receivers by value
  - [x] pass pointer receivers by address from `value.method(...)`
  - [x] preserve generic monomorphisation for methods
- [x] Formatter/editor support:
  - [x] format impl blocks
  - [x] lex `impl` as a keyword for editor token streams
- [x] Tests:
  - [x] language tests for local inherent methods
  - [x] language tests for imported generic inherent methods
  - [x] error tests for invalid impl members
- [x] Documentation:
  - [x] manual section for inherent methods
  - [x] syntax-reference appendix entries
  - [x] language-reference appendix rules
- [x] Standard library migration:
  - [x] convert `std.collections.Stack` helpers to inherent methods
  - [x] update examples that use `Stack`

### Traits Milestone

- [ ] Add traits as a simple interface mechanism for types.
- [ ] Use the `trait` keyword rather than `interface`.
- [ ] Define the language-known built-in traits in `std.traits`:
  - [ ] `Display`
  - [ ] `Eq`
  - [ ] `Order`
  - [ ] `Default`
  - [ ] `Iterator`
- [ ] Treat built-in traits as normal canonical standard-library declarations
  where practical, with compiler recognition based on module path and name
  rather than unqualified spelling.
- [ ] Add trait declaration syntax:
  - [ ] `Name :: trait { ... }`
  - [ ] generic traits such as `Name :: trait [Item] for Value { ... }`
  - [ ] optional named self form `Name :: trait for Value { ... }`
  - [ ] make `Self` available inside every trait body
  - [ ] make the named self alias available only inside that trait body
  - [ ] document that authors should normally use either `Self` or the named
    self alias consistently within one trait
- [ ] Support required function members only in the first version:
  - [ ] no default method bodies
  - [ ] no associated types
  - [ ] no associated constants
  - [ ] no trait objects or dynamic dispatch
- [ ] Add implementation syntax:
  - [ ] `impl TraitName for Type { ... }`
  - [ ] allow implementations for plex, union, enum, and primitive types
  - [ ] allow generic implementations such as `impl [T] Eq for []T where T: Eq`
  - [ ] reject overlapping implementations in the first version
- [ ] Add built-in trait semantics:
  - [ ] `Display` supplies text conversion for interpolation
  - [ ] defer interpolation formatting specifiers such as `{expr; format}`
  - [ ] `Eq` supports equality semantics
  - [ ] `Order` supports comparison semantics through a `compare`-style member
  - [ ] `Default` customises default initialisation for typed bindings
  - [ ] `Iterator[Item]` supplies `next :: fn (iter: ^Iter) -> Option[Item]`
- [ ] Add `Default` initialisation rules:
  - [ ] `x: T` uses `Default[T].default()` when an implementation exists
  - [ ] `x: T` falls back to zero-initialisation when no `Default`
    implementation exists
  - [ ] `x: T = undefined` remains the explicit way to opt out of
    initialisation
- [ ] Add `Iterator` loop rules:
  - [ ] use one `Iterator` trait only in the first version
  - [ ] `for elem in iter` binds the payload from `Option[Item]`
  - [ ] `for ^elem in iter` works when the iterator item is a pointer type
    such as `^T`
  - [ ] keep built-in collection iteration working for arrays, slices, dynamic
    arrays, and ranges
  - [ ] user-defined non-iterator collections can expose iterator-producing
    functions; do not add a separate `Iterable` trait in this milestone
- [ ] Add generic constraint syntax:
  - [ ] `where T: TraitName`
  - [ ] support multiple trait constraints in a `where` clause
  - [ ] interpret constraints as requirements that matching implementations
    exist at instantiation time
- [ ] Add trait member call syntax:
  - [ ] receiver form `<value>.<trait_fn>(...)`
  - [ ] explicit form `<Trait>.<trait_fn>(value, ...)`
  - [ ] explicit implementation form `<Trait>[Type].<trait_fn>(...)` when the
    implementation type cannot be inferred from a receiver argument, such as
    `Default[Foo].default()`
  - [ ] resolve receiver calls through matching trait implementations
  - [ ] prefer existing direct field/member behaviour before trait lookup
  - [ ] report ambiguity when multiple traits provide the same receiver method
    for a type
  - [ ] require explicit trait calls for functions that cannot be resolved from
    a receiver value
- [ ] Parser and AST work:
  - [ ] parse trait declarations
  - [ ] parse trait generic parameter lists
  - [ ] parse optional named self aliases
  - [ ] parse impl blocks
  - [ ] parse `where` clauses on generic functions and impl blocks
- [ ] Sema work:
  - [ ] register trait declarations and required members
  - [ ] substitute `Self` and named self aliases when checking impl members
  - [ ] require impls to provide all required trait functions
  - [ ] reject impl members with incompatible signatures
  - [ ] validate `where` constraints against known traits
  - [ ] Add stricter non-lazy generic body checks once constraints exist:
    unresolved names that do not depend on concrete type arguments should be
    rejected before a generic function or impl method is instantiated, while
    constraint-sensitive operations should be checked against trait bounds.
  - [ ] prove trait requirements for generic instantiations
  - [ ] reject missing, duplicate, overlapping, or ambiguous implementations
  - [ ] provide useful `help` text when trait generic parameters are required
    but cannot be inferred
- [ ] IR/C generation work:
  - [ ] lower trait calls to statically selected concrete functions
  - [ ] keep trait declarations out of generated C output
  - [ ] emit implementation functions with stable generated C names
  - [ ] preserve monomorphisation behaviour for generic impls
- [ ] Formatter work:
  - [ ] format trait declarations
  - [ ] format named self aliases
  - [ ] format impl blocks
  - [ ] format `where` clauses
  - [ ] format receiver and explicit trait member calls
- [ ] LSP/editor work:
  - [ ] hover for trait declarations and impl members
  - [ ] go-to-definition from trait member calls to selected impl members where
    possible
  - [ ] semantic tokens for `trait`, `impl`, `where`, `Self`, and trait member
    declarations
  - [ ] document symbols for traits and impl blocks
- [ ] Tests:
  - [ ] language tests for trait declarations using `Self`
  - [ ] language tests for trait declarations using `trait for Value`
  - [ ] language tests for plex implementations
  - [ ] language tests for union, enum, and primitive implementations
  - [ ] language tests for receiver-form trait calls
  - [ ] language tests for explicit trait calls
  - [ ] language tests for generic constraints using `where T: Trait`
  - [ ] language tests for generic impls
  - [ ] language tests for built-in `Display`, `Eq`, `Order`, `Default`, and
    `Iterator`
  - [ ] language tests for `Default` overriding typed-binding initialisation
  - [ ] language tests for zero-initialisation fallback when no `Default`
    implementation exists
  - [ ] language tests for `Iterator` returning `Option[Item]`
  - [ ] language tests for `for elem` and `for ^elem` item typing
  - [ ] error tests for missing required impl members
  - [ ] error tests for incompatible impl member signatures
  - [ ] error tests for unknown traits in constraints or impls
  - [ ] error tests for duplicate or overlapping impls
  - [ ] error tests for ambiguous receiver trait calls
  - [ ] error tests for non-inferable trait generic parameters with useful
    `help` messages
  - [ ] format tests for trait and impl syntax
  - [ ] LSP tests for hover/symbol/token behaviour affected by traits
- [ ] Documentation:
  - [ ] manual section introducing traits as interfaces for types
  - [ ] manual examples for `Self` and `trait for Value`
  - [ ] manual examples for `impl Trait for Type`
  - [ ] manual examples for receiver and explicit trait calls
  - [ ] manual examples for generic constraints using traits
  - [ ] manual section for built-in traits in `std.traits`
  - [ ] manual examples for `Default` initialisation and zero-init fallback
  - [ ] manual examples for `Iterator` and `Option[Item]`
  - [ ] standard-library documentation for `Option[T]`, `Result[T, E]`, and
    `std.traits`
  - [ ] syntax-reference appendix entries
  - [ ] language-reference appendix rules

### LSP Completion And Incremental Sync Milestone

- [x] Improve LSP document sync performance:
  - [x] advertise incremental document sync instead of full-document sync
  - [x] keep one internal text buffer per open document
  - [x] apply `didChange` range edits to the internal buffer
  - [x] keep re-running the existing full front-end analysis after edits until
    true incremental lex/parse/sema work is designed
  - [x] preserve diagnostics for edited buffers even when analysis fails
- [x] Add completion support:
  - [x] advertise `completionProvider`
  - [x] handle `textDocument/completion`
  - [x] complete language keywords
  - [x] complete visible top-level declarations
  - [x] complete visible locals and parameters
  - [x] complete fields and inherent methods after `value.`
  - [x] complete module paths after `use`
  - [x] complete public module symbols after module member access
- [x] Add signature-help support:
  - [x] advertise `signatureHelpProvider`
  - [x] show callable signatures for `foo(|)`
  - [x] include default parameter values and named-argument guidance
- [x] Tests:
  - [x] LSP tests for advertised incremental sync and completion capabilities
  - [x] LSP tests for incremental `didChange` edits
  - [x] LSP tests for keyword completion
  - [x] LSP tests for top-level and local symbol completion
  - [x] LSP tests for field and method completion
  - [x] LSP tests for module path completion
  - [x] LSP tests for signature help
- [x] Editor integration:
  - [x] verify VS Code completion trigger behaviour
  - [x] verify Neovim completion trigger behaviour
  - [x] update editor extension/plugin configuration if needed

### Source Testing Milestone

- [x] Reserve `nerd test <root-filename>` for unit tests declared in Nerd
  source code.
- [x] Move the current compiler regression harness out of the `nerd`
  executable:
  - [x] port language/error/format/LSP/command test orchestration to Python
  - [x] keep existing test file formats during the migration
  - [x] update `just test` / `just t` to run the Python harness
  - [x] remove `src/testing` and the old regression `compiler_cmd_test` path
    from the production compiler binary once Python owns the harness
- [x] Add `nerd test` CLI options for source tests:
  - [x] required positional root source filename
  - [x] `--filter <text>` to run only matching test names
  - [x] `--list` to list discovered tests without running them
- [x] Add source-level test syntax:
  - [x] `test "name" { ... }`
  - [x] top-level/module-level only
  - [x] test blocks type-check like `void` functions
  - [x] test declarations do not become part of normal module APIs
- [x] Add test discovery:
  - [x] load the root module and imports for normal visibility
  - [x] collect root-module test declarations
  - [x] apply `--filter` before execution
  - [x] make `--list` output stable, readable test names
- [x] Add parser and runner support:
  - [x] parse test declarations
  - [x] store test name and body for the root module during test generation
- [x] Add sema support:
  - [x] type-check selected test bodies with module-scope visibility
  - [x] reject tests in invalid locations
  - [x] keep duplicate names valid for now; filters and lists preserve source
    order for stable output
- [x] Add lowering and runner support:
  - [x] generate one hidden function per selected test
  - [x] generate a test main that calls discovered tests
  - [x] report pass/fail counts
  - [x] return nonzero if any selected test fails
- [x] Decide and implement assertion behaviour for test mode:
  - [x] accept v1 fail-fast behaviour from current `assert`
  - [x] keep non-fail-fast test assertions as follow-up work if continuing
    after failure becomes useful
- [x] Tests for the feature:
  - [x] command tests for `nerd test root.n`
  - [x] command tests for `nerd test root.n --filter text`
  - [x] command tests for `nerd test root.n --list`
  - [x] command tests for valid test declarations
  - [x] formatter test for `test "name" { ... }`
  - [x] error tests for invalid test declarations
  - [x] regression tests proving compiler harness execution no longer depends
    on `nerd test`
- [x] Documentation:
  - [x] manual section for source tests
  - [x] syntax-reference appendix entry
  - [x] language-reference appendix rules
  - [x] update contributor/test documentation for the Python compiler harness

Follow-up source testing work:

- [ ] collect and run tests declared in imported modules
- [ ] add non-fail-fast test assertions if continuing after failure becomes
  useful

### Editor Intelligence Milestone

- [ ] Keep extending formatter support as syntax lands.
- [ ] Keep extending LSP support as syntax and semantics land.
- [ ] Treat VS Code and Neovim/LazyVim as paired editor surfaces. When a change
  affects file detection, syntax highlighting, LSP launch/configuration,
  formatting, or install packaging, update both editor integrations in the same
  commit unless one is explicitly out of scope.
- [ ] Prioritise semantic LSP intelligence before Tree-sitter. The compiler
  front end already has semantic information that should drive completion,
  signature help, definitions, references, rename, and code actions.
- [ ] Add LSP completion:
  - [ ] keywords and snippets by syntactic context
  - [ ] built-in types and common type forms
  - [ ] visible locals, parameters, globals, imported declarations, and module
    members
  - [ ] plex/union fields after field access
  - [ ] enum variants where the expected type is known
- [ ] Add signature help for function calls, triggered by `(` and `,`, including
  FFI and imported functions.
  - [ ] ordinary function calls
  - [ ] default parameter display
  - [ ] FFI function calls
  - [ ] imported function calls
- [ ] Add references and rename:
  - [ ] same-file references
  - [ ] same-file rename
  - [ ] cross-module references
  - [ ] cross-module rename
- [ ] Add code actions for diagnostics where the compiler already has actionable
  help, such as `:` versus `::`, unused local fixes, and links to
  `nerd explain <code>`.
  - [ ] `:` versus `::` function-definition fix
  - [ ] unused local fixes
  - [ ] `nerd explain <code>` links or commands
- [ ] Add document links for module imports.
- [ ] Add workspace symbol search for top-level declarations.
- [ ] Add Tree-sitter support after the LSP intelligence work is useful:
  - [ ] grammar for Nerd source
  - [ ] editor-native highlighting
  - [ ] folding
  - [ ] indentation
  - [ ] incremental selection
  - [ ] structural editing support, especially for Neovim

## Deferred Work

These items are worth keeping visible, but they should not be treated as
near-term tasks without a fresh design pass.

- In-place AST compaction for constant folding.
- Calling-convention annotations for FFI, including platform-specific behaviour.
- Named arguments, including their interaction with default parameters and
  call-site argument ordering.
- Numeric generic parameters, such as `[T, N: usize]`, including canonical
  value identity and constant-evaluation rules.
- Targeted diagnostics for value arguments in generic type-argument positions
  and type arguments in value-index positions.
- Truncating readable generic C symbol stems while keeping the hash suffix for
  collision resistance.
- Interpolation formatting specifiers such as `{expr; format}`.
- Operator traits for arithmetic, bitwise, comparison, and assignment
  operators.
- A synthetic diagnostic harness for hard-limit error categories that are
  impractical to trigger through normal source files, including `0102`, `0105`,
  and `0200`.
- Retiring `TODO.md` once its ideas have been integrated into active roadmap
  work or deliberately discarded.
