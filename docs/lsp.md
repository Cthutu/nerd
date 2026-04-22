# LSP System

The language server lives in `src/lsp` and is intentionally thin over the
compiler front end.

The key files are:

- [src/lsp/lsp.h](/home/matt/nerd/src/lsp/lsp.h)
- [src/lsp/lsp.c](/home/matt/nerd/src/lsp/lsp.c)
- [src/lsp/document.c](/home/matt/nerd/src/lsp/document.c)
- [src/lsp/hover.c](/home/matt/nerd/src/lsp/hover.c)

## Core Design

Each open document stores:

- an arena for document-owned allocations
- `FrontEndState`
- `Cst`
- flags telling whether semantic analysis and CST parsing succeeded

This means LSP features do not maintain a separate semantic model. They reuse
the compiler front end directly.

## Document Lifecycle

`didOpen` and `didChange` both:

1. replace the document contents
2. re-run front-end analysis
3. attempt CST parsing
4. publish diagnostics

`didClose` clears the stored document and publishes an empty diagnostic set.

## Diagnostics Flow

The diagnostics path is tightly coupled to the compiler error system:

1. LSP switches the error system to `ERROR_RENDER_DIAGNOSTICS`
2. front-end analysis runs with output emission disabled
3. if analysis fails, the last rendered diagnostics JSON is parsed
4. the parsed array is sent as `textDocument/publishDiagnostics`

That reuse keeps compiler diagnostics and LSP diagnostics aligned.

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
