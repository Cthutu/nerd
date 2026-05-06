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

The server advertises incremental document sync and applies LSP range edits to
the stored buffer. Analysis is still whole-document: each accepted edit reruns
the existing front end until a real incremental lexer/parser/sema design lands.

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

Definition jumps resolve through semantic declaration indices and then convert
the binding token span back into an LSP range.

Rename supports same-document semantic renames for locals and local
top-level declarations. It edits only token ranges that belong to the open
editor document, so imported declarations and generated folder-module sibling
content are deliberately left out until workspace-wide edits are implemented.

## Completion And Signature Help

Completion is semantic where possible:

- normal positions offer language keywords plus visible declarations, locals,
  and parameters
- `value.` offers fields for plexes/unions, built-in fields for strings,
  slices, and dynamic arrays, and inherent methods for matching receiver types
- `module_binding.` offers public exports for `module_binding :: use ...`
  imports, including while the edited document or imported module has parse or
  semantic errors. Folder-module fallback includes public declarations from
  sibling module-part files, while still skipping part files that `mod.n`
  explicitly imports as child modules. This fallback relies on the stored
  document URI remaining valid across requests, because relative module imports
  are resolved from the active document path.
- `use ...` offers module path segments from the active module search roots

The dynamic-array member list is shared conceptually with semantic analysis and
should include the built-in fields `data`, `count`, `capacity`, plus methods
`append`, `clear`, `free`, `pop`, `push`, `reserve`, `resize`, and
`resize_undefined`.

Completion results are filtered by the server using the exact typed prefix.
Matching is case-sensitive, matching Nerd symbol resolution.

Signature help is triggered by `(` and `,`. It resolves the callable name through
the semantic declaration table, reports the active argument, includes default
parameter expressions, and reminds the editor user of named-argument syntax.

## Code Actions

The server offers a quick fix for incomplete plex literals. When a literal is
missing fields, `Fill missing plex fields` inserts the remaining fields with
simple default values such as `0`, `no`, `""`, and `nil`, with field colons
aligned to the plex field group. Empty literals insert the first generated field
directly after the opening brace line, without an extra blank row. The action
first uses semantic type information and then falls back to the AST for local
plex aliases, or imported module ASTs, so it remains available while the
missing-field diagnostic is present and while the cursor is on either the literal
target or the literal body.

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
