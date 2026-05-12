# LSP Boundary Audit

Status: started

## Current Shape

The LSP document snapshot currently owns:

- editor buffer source in `source_arena`
- compiler analysis arena
- `ProgramInfo`
- root `FrontEndState`
- CST
- `analysis_ok`
- `semantic_ready`
- `has_cst`

Document open/change runs the compiler front end with:

```c
FrontEndOptions options = {
    .require_entry_point = false,
    .skip_hir_generation = true,
    .keep_partial_results = true,
};
```

This is the correct high-level instinct for editor tooling: avoid IR, do not
require a build entry point, and keep useful partial products. The weak part is
that the products do not currently describe what is actually valid.

## Observed Product Boundaries

### Source

`doc->source` is the authoritative editor buffer. Incremental changes are
applied by range replacement against this string.

Good:

- feature requests use source positions from the live editor buffer
- failed analysis does not discard the current buffer

Risks:

- LSP range inputs outside the current buffer are clamped to end-of-source by
  `lsp_offset_from_position`
- features sometimes compare `doc->source` with the analysed lexer source
  because folder modules may analyse a combined source

Current boundary:

- Feature handlers request `LspSourceView` when source is the minimum useful
  product, for example completion and rename.

### Tokens

Tokens live in `doc->front_end.lexer.tokens`. Most source-level requests need
tokens before they need semantic facts.

Good:

- semantic tokens, hover, completion, rename, and signature help all naturally
  start from token/range lookup

Risks:

- there is no explicit `tokens_ready` flag
- callers infer token availability from `semantic_ready` or array counts

### AST

The compact AST is the main source structure used by LSP features. CST exists
but most feature implementations still walk AST and sema side tables directly.

Good:

- AST node arrays are dense and easy to scan
- many features can fall back to syntax-only walks

Risks:

- parse failure can leave no AST
- partial AST does not have a declared error/recovery contract
- syntax context for formatter/LSP is split between AST and CST

### CST

`doc->cst` is parsed separately from the front-end AST and guarded by
`cst_ready`.

Good:

- provides a source-preserving product for tooling
- lets tooling grow separately from compiler AST

Risks:

- separate parser/product means syntax changes can diverge
- most LSP features do not currently use it as their primary source structure

### Semantic Facts

`doc->front_end.sema` contains declarations, locals, scopes, types, and
AST-indexed side tables. LSP features read these arrays directly.

Good:

- rich enough to power completion, hover, definition, rename, and code actions
- partial results are already kept for editor use

Resolved during this review:

- `semantic_ready` has been split into `tokens_ready`, `syntax_ready`,
  `sema_partial`, and `sema_complete`.
- `has_cst` has been renamed to the product-oriented `cst_ready`.
- The compiler-owned `FrontEndState` now carries explicit product readiness for
  lexer, AST, sema, and HIR. The LSP document derives its feature-facing
  booleans from those states instead of inferring validity from array counts.

Remaining risks:

- there is still no distinction between declaration facts, binding facts, and
  checked type facts
- imported module views currently prove only that a module row exists and expose
  checked export-declaration lookup; they do not yet distinguish imported
  token, syntax, declaration, or type readiness

## Feature Contracts

Current minimum products should be made explicit:

| Feature | Minimum useful product | Better product |
| --- | --- | --- |
| diagnostics | source + compiler diagnostics | grouped per module URI |
| semantic tokens | tokens | tokens + checked decl/type facts |
| document symbols | AST or tolerant CST | declaration index |
| completion | source + tokens | decl index + type facts + module index |
| hover | token at position | binding/type facts |
| definition | token at position | binding/reference index |
| rename | token + source ranges | binding/reference index |
| signature help | tokens + call syntax | resolved callee/function facts |
| code actions | syntax/ranges | targeted type/declaration facts |

The important architectural point is that no feature should receive a raw
`LspDocument*` and then guess which fields are safe. A request should ask for a
named product or accessor and receive either a valid view or no result.

## Recommended Interfaces

### Readiness States

The compiler front end exposes product states on `FrontEndState`:

```c
typedef enum : u8 {
    FRONT_END_PRODUCT_Missing,
    FRONT_END_PRODUCT_Partial,
    FRONT_END_PRODUCT_Complete,
} FrontEndProductState;

typedef struct FrontEndReadiness {
    FrontEndProductState lexer;
    FrontEndProductState ast;
    FrontEndProductState sema;
    FrontEndProductState hir;
} FrontEndReadiness;
```

Current semantics:

| Product | Complete | Partial | Missing |
| --- | --- | --- | --- |
| lexer | `lex` succeeded | reserved for future token recovery | no usable token stream |
| AST | parser produced syntax nodes | reserved for tolerant AST recovery | no usable AST |
| sema | semantic analysis fully succeeded | editor tooling may enter checked fallback paths, but callers must treat every sema side-table lookup as optional | no semantic view should be exposed |
| HIR | HIR generation completed | not currently used | no usable HIR |

The LSP document still carries product-oriented booleans for compact request
guards, but those booleans are now derived from the compiler-owned readiness
states:

```c
bool source_ready;
bool tokens_ready;
bool syntax_ready;
bool sema_partial;
bool sema_complete;
bool cst_ready;
```

`cst_ready` remains LSP-owned because CST parsing is currently an editor-tooling
product rather than part of the compiler front-end pipeline.

For now the LSP upgrades a failed post-parse analysis to partial sema
readiness, preserving existing syntax-fallback behavior in hover, definition,
rename, completion, and code actions. This is intentionally weaker than checked
semantic analysis: any direct table lookup must go through checked accessors.
Milestone 5's next slice should replace this broad partial state with narrower
fact views.

### Checked Accessors

Accessors around semantic side tables are now available:

```c
bool lsp_ast_node(const Ast* ast, u32 node_index, const AstNode** out);
bool lsp_lexer_token(const Lexer* lexer, u32 token_index, const Token** out);
bool lsp_token_range(const Lexer* lexer, u32 token_index, usize* start, usize* end);
bool lsp_sema_decl(const Sema* sema, u32 decl_index, const SemaDecl** out);
bool lsp_sema_node_decl(const Sema* sema, u32 node_index, u32* out_decl_index);
bool lsp_sema_node_local(const Sema* sema, u32 node_index, u32* out_local_index);
bool lsp_sema_node_scope(const Sema* sema, u32 node_index, u32* out_scope_index);
bool lsp_sema_node_type(const Sema* sema, u32 node_index, u32* out_type_index);
bool lsp_sema_local(const Sema* sema, u32 local_index, const SemaLocal** out);
bool lsp_sema_scope(const Sema* sema, u32 scope_index, const SemaScope** out);
bool lsp_sema_type(const Sema* sema, u32 type_index, const SemaType** out);
bool lsp_sema_decl_by_symbol(const Sema* sema, u32 symbol, const SemaDecl** out, u32* out_index);
bool lsp_sema_type_param(const Sema* sema, u32 param_index, u32* out_symbol, u32* out_type_index);
```

Semantic tokens, rename, signature help, code actions, and some hover helpers
have started using these accessors.

Semantic token classification now receives an `LspSemanticView` and reaches AST
nodes, tokens, declarations, and token ranges through the checked helpers. The
next LSP slices should keep moving hover, definition, rename, completion, and
code actions from raw `doc->front_end` access to these view/accessor APIs.

### Analysis Snapshot Views

Feature-facing views now exist for source, token, syntax, and semantic products:

```c
typedef struct {
    const LspDocument* doc;
    string source;
} LspSourceView;

typedef struct {
    const LspDocument* doc;
    string source;
    const Lexer* lexer;
} LspTokenView;

typedef struct {
    const LspDocument* doc;
    string source;
    const Lexer* lexer;
    const Ast* ast;
} LspSyntaxView;

typedef struct {
    const LspDocument* doc;
    string source;
    const Lexer* lexer;
    const Ast* ast;
    const Sema* sema;
} LspSemanticView;

typedef struct {
    const ModuleInfo* info;
    u32 module_index;
    const Lexer* lexer;
    const Ast* ast;
    const Sema* sema;
} LspModuleView;
```

The constructor for each view checks readiness once. Feature handlers now use
these boundaries instead of directly looking up documents from the document
map:

- completion: `LspSourceView`
- rename: `LspSourceView`, with an internal scratch syntax fallback for
  partially parsed source
- semantic tokens: `LspTokenView`
- document symbols: `LspSemanticView`
- hover and definition: `LspSemanticView`
- signature help: `LspSemanticView`
- code actions: `LspSemanticView`

Direct document-map lookup is now isolated to document/view construction and
document lifecycle operations.

Imported-module access now also goes through checked helpers:

- `lsp_program_module_view`
- `lsp_program_module_view_by_path`
- `lsp_program_module_view_by_type`
- `lsp_module_export_decl`

Completion, hover/definition, and code actions use these helpers for imported
module exports and module file locations. Remaining direct `ProgramInfo` access
in LSP is lifecycle/root-module staging, rename's local/import filtering, and
scratch program setup for syntax-export fallback completion.

Signature help now keeps its helper pipeline on `LspSemanticView` instead of
reaching back through `LspDocument` for lexer, AST, sema, and source access.
The same pass added bounds checks around signature AST nodes, FFI signature
indices, generic parameter symbols, function parameter ranges, return type
tokens, and type-parameter arrays.

## Open Questions

- Should declaration/scope indexing become a guaranteed product even when type
  checking fails?
- Should the CST become the primary source structure for document symbols,
  ranges, and rename preparation?
- Can the compiler front end expose partial products directly instead of
  relying on global error state plus `keep_partial_results`?
- Should imported module readiness be tracked separately from current-document
  readiness?
- Which completion fallbacks become unnecessary once declaration/type readiness
  is explicit?

## Next Actions

1. Decide whether declaration and binding readiness need separate flags.
2. Decide whether imported module views need token/syntax/sema readiness levels
   beyond "module row exists".
3. Decide whether hover internals should be converted from `LspDocument*`
   helper plumbing to `LspSemanticView` plumbing. The entry point is already
   view-gated, so this is now hardening work rather than a feature boundary
   blocker.
4. Keep adding stress cases for chained edits, broken imports, incomplete type
   syntax, rename, and semantic tokens.
