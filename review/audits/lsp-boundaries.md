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
    .skip_ir_generation = true,
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

Remaining risks:

- there is still no distinction between declaration facts, binding facts, and
  checked type facts
- direct side-table access still exists in hover, completion, and imported
  module helpers

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

### Readiness Flags

The first implementation step replaced the current loose booleans with explicit
product readiness fields on `LspDocument`:

```c
bool source_ready;
bool tokens_ready;
bool syntax_ready;
bool sema_partial;
bool sema_complete;
bool cst_ready;
```

These fields are set by document staging after each product is produced. A
future bitset or view constructor may still be useful, but the first step is
now in the codebase.

### Checked Accessors

Accessors around semantic side tables are now available:

```c
bool lsp_sema_decl(const Sema* sema, u32 decl_index, const SemaDecl** out);
bool lsp_sema_node_decl(const Sema* sema, u32 node_index, u32* out_decl_index);
bool lsp_sema_node_local(const Sema* sema, u32 node_index, u32* out_local_index);
bool lsp_sema_node_type(const Sema* sema, u32 node_index, u32* out_type_index);
bool lsp_sema_local(const Sema* sema, u32 local_index, const SemaLocal** out);
bool lsp_sema_type(const Sema* sema, u32 type_index, const SemaType** out);
```

Semantic tokens, rename, signature help, code actions, and some hover helpers
have started using these accessors.

### Analysis Snapshot Views

Introduce small feature-facing views instead of exposing all of `LspDocument`:

```c
typedef struct {
    string source;
    const Lexer* lexer;
} LspTokenView;

typedef struct {
    string source;
    const Lexer* lexer;
    const Ast* ast;
    const Sema* sema;
} LspSemanticView;
```

The constructor for each view checks readiness once. Feature handlers then
work against the narrowest view they need.

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

1. Continue migrating hover and completion direct side-table reads.
2. Add feature-facing view constructors for token, syntax, and semantic views.
3. Decide whether declaration and binding readiness need separate flags.
4. Keep adding stress cases for chained edits, broken imports, incomplete type
   syntax, rename, and semantic tokens.
