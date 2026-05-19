# LSP System

The language server lives in `src/lsp` and is intentionally thin over the
compiler front end.

The key files are:

- [src/lsp/lsp.h](/home/matt/nerd/src/lsp/lsp.h)
- [src/lsp/lsp.c](/home/matt/nerd/src/lsp/lsp.c)
- [src/lsp/document.c](/home/matt/nerd/src/lsp/document.c)
- [src/lsp/hover.c](/home/matt/nerd/src/lsp/hover.c)
- [src/lsp/completion.c](/home/matt/nerd/src/lsp/completion.c)
- [src/lsp/signature.c](/home/matt/nerd/src/lsp/signature.c)
- [src/lsp/semantic_tokens.c](/home/matt/nerd/src/lsp/semantic_tokens.c)
- [src/lsp/code_action.c](/home/matt/nerd/src/lsp/code_action.c)
- [src/lsp/rename.c](/home/matt/nerd/src/lsp/rename.c)

## Core Design

Each open document stores:

- an arena for document-owned allocations
- the current editor buffer text
- a stable copy of the document URI/source path for module resolution
- `FrontEndState`
- `Cst`
- flags telling whether semantic analysis and CST parsing succeeded

This means LSP features do not maintain a separate semantic model. They reuse
the compiler front end directly.

## Document Lifecycle

`didOpen` and `didChange` both:

1. update the internal document contents
2. re-run front-end analysis
3. attempt CST parsing
4. publish diagnostics
5. re-run analysis for the other open documents

The server advertises incremental document sync and applies LSP range edits to
the stored buffer. Analysis is still whole-document: each accepted edit reruns
the existing front end until a real incremental lexer/parser/sema design lands.
When analysis loads an imported module, the LSP first checks whether that module
is already open in the editor and uses the open buffer text before falling back
to the file on disk. This keeps diagnostics and quick fixes responsive when one
open module starts exporting a type that another open module already imports.

`didClose` clears the stored document and publishes an empty diagnostic set.

## Diagnostics Flow

The diagnostics path is tightly coupled to the compiler error system:

1. LSP switches the error system to `ERROR_RENDER_DIAGNOSTICS`
2. front-end analysis runs with output emission disabled
3. if analysis fails, the last rendered diagnostics JSON is parsed
4. the parsed array is grouped by physical source URI
5. each group is sent as its own `textDocument/publishDiagnostics`

That reuse keeps compiler diagnostics and LSP diagnostics aligned.
When folder modules are expanded from `mod.n` plus part files, diagnostics are
mapped back to the physical part file before LSP ranges are emitted.
Imported module diagnostics are also published under the imported file's URI,
not the URI of whichever open document triggered analysis. The active document
is cleared separately so VS Code's Problems list and next-error navigation do
not show the same imported error under several files.

## Hover And Navigation

The current hover and definition implementation is mostly token-driven:

- map the request position to a lexer token
- map that token to an AST node when needed
- map that node to semantic declaration or type data

Hover uses semantic tables for:

- declaration kind
- inferred type text
- function signature text
- constant integer values when they can be evaluated

For field access hover and definition, the server falls back to AST type
annotations when semantic analysis stops before attaching receiver type data.
This keeps `param.field` useful while code is mid-edit, including cases where
the field access itself is part of the current diagnostic.

Definition jumps resolve through semantic declaration indices and then convert
the binding token span back into an LSP range.

Rename supports same-document semantic renames for locals and local
top-level declarations, including constant and mutable variable bindings. When
semantic analysis fails but the AST is still available, rename falls back to
same-file binding/reference tokens with the same symbol so simple renames keep
working while code is mid-edit. If imported-module analysis fails before the
front end preserves a root AST, rename lexes and parses just the open buffer for
that same syntax fallback. It edits only token ranges that belong to the open
editor document, so imported declarations and generated folder-module sibling
content are deliberately left out until workspace-wide edits are implemented.

## Completion And Signature Help

Completion is semantic where possible:

- normal positions offer language keywords plus visible declarations, locals,
  and parameters
- `value.` offers fields for plexes/unions, built-in fields for strings,
  slices, and dynamic arrays, and inherent methods for matching receiver types
  from semantic analysis. When later parse or semantic errors make full
  analysis unavailable, completion falls back to AST and light source-text
  recovery for declarations, function parameters, and `for item in collection`
  binders that appear before the error, including loop expressions used as
  `return for ...`.
- `module_binding.` offers public exports for `module_binding :: use ...`
  imports, including while the edited document or imported module has parse or
  semantic errors. Folder-module fallback includes public declarations from
  sibling module-part files, while still skipping part files that `mod.n`
  explicitly imports as child modules. This fallback relies on the stored
  document URI remaining valid across requests, because relative module imports
  are resolved from the active document path.
- `payload.` inside an `on` branch can complete fields for an enum payload
  binder such as `Variant(as payload) => { payload. }` even while the member
  access is syntactically incomplete. Imported enum payloads such as
  `event : module.Event` use the same module-part fallback as module completion
  so payload fields remain available from public plex types in imported files.
  The fallback repairs only the transient completion analysis buffer; it does
  not mutate the open document.
- `use ...` offers module path segments from the active module search roots

The dynamic-array member list is shared conceptually with semantic analysis and
should include the built-in fields `data`, `count`, `capacity`, plus methods
`append`, `clear`, `delete`, `extend`, `extend_undefined`, `free`, `pop`,
`push`, `reserve_extra`, `reserve_to`, `resize_to`, `resize_undefined_to`, and
`swap_delete`.

Completion results are filtered by the server using the exact typed prefix.
Matching is case-sensitive, matching Nerd symbol resolution.
The VS Code client marks Nerd completion lists as incomplete so the editor
re-queries the server as the user continues typing; this prevents VS Code's
cached, case-insensitive client filtering from showing stale mismatched items.

Signature help is triggered by `(` and `,`. It resolves the callable name through
the semantic declaration table, reports the active argument, includes default
parameter expressions, and reminds the editor user of named-argument syntax.

## Semantic Tokens

Semantic tokens are produced from lexer tokens and enriched with AST and semantic
tables where possible. The server advertises the standard `unnecessary` token
modifier and applies it to module path symbols in `use` statements when that
import does not contribute a declaration or local binding referenced by the
current file. Editors such as VS Code usually render that modifier as dimmed
text. The diagnostics path also publishes a hint diagnostic with
`DiagnosticTag.Unnecessary` for the same range, because clients generally use
that tag for faded unused code even when a theme ignores semantic-token
modifiers.

## Code Actions

The server offers a quick fix for incomplete plex literals. When a literal is
missing fields, `Fill missing plex fields` inserts the remaining fields with
the same default values that `...` would use, such as `0`, `no`, `""`, and
`nil`, with field colons aligned to the plex field group. Empty literals insert
the first generated field directly after the opening brace line, without an
extra blank row. The action first uses semantic type information and then falls
back to the AST for local plex aliases, or imported module ASTs, so it remains
available while the
missing-field diagnostic is present and while the cursor is on either the literal
target or the literal body.

The server also offers import quick fixes for unresolved symbols. It searches
loaded modules, sibling modules beside the active/root document, and the
available `mods` tree for public exports with the missing name, then inserts
`use module.path` either at the top of the file or after the first leading group
of `use` statements. Existing imports are filtered out before actions are
returned, so a stale unresolved-symbol diagnostic cannot offer a duplicate
`use`.

## CST Usage

The CST is used where token-accurate source structure matters more than semantic
meaning. In the current LSP implementation that mainly affects document-symbol
style features and keeps formatting/editor tooling separate from semantic data.

## Practical Rule

If a new language feature adds compiler-visible names or types, update the LSP
horizontally:

- diagnostics
- hover text
- definition behaviour
- semantic tokens or document symbols where relevant

The LSP should not drift behind the compiler for implemented language features.
