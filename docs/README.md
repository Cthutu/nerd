# Developer Documentation

This folder contains developer-facing documentation for the repository.

## Source Of Truth

Use [../ROADMAP.md](/home/matt/nerd/ROADMAP.md) for current project direction,
active work, and cross-cutting engineering rules. Use
[manual/README.md](/home/matt/nerd/docs/manual/README.md) and the manual
appendices for source-level language behaviour.

The other documents in this folder are implementation notes. Keep them when
they describe compiler structure, subsystem ownership, diagnostics, tooling, or
standard-library surface that is not already covered by the manual. When one of
these notes disagrees with the roadmap, the manual, tests, or the implementation,
verify against the implementation and update the note before relying on it.

## Reading Order

If you are new to the codebase, read these first:

1. [overviews/INTERNALS.md](/home/matt/nerd/docs/overviews/INTERNALS.md)
2. [compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
3. [error-system.md](/home/matt/nerd/docs/error-system.md)
4. [type-system.md](/home/matt/nerd/docs/type-system.md)
5. [string-runtime.md](/home/matt/nerd/docs/string-runtime.md)
6. [stdlib.md](/home/matt/nerd/docs/stdlib.md)
7. [lsp.md](/home/matt/nerd/docs/lsp.md)
8. [editor-support.md](/home/matt/nerd/docs/editor-support.md)
9. [testing.md](/home/matt/nerd/docs/testing.md)

If you are learning the Nerd language rather than the compiler implementation,
start with [manual/README.md](/home/matt/nerd/docs/manual/README.md). If you are
validating the language against the implementation, use
[spec/README.md](/home/matt/nerd/docs/spec/README.md).

## Overviews

- [overviews/INTERNALS.md](/home/matt/nerd/docs/overviews/INTERNALS.md)
  High-level architecture and subsystem map.
- [overviews/FORMAT.md](/home/matt/nerd/docs/overviews/FORMAT.md)
  Current formatter rules and behaviour.
- [overviews/build-directives.md](/home/matt/nerd/docs/overviews/build-directives.md)
  Build/module directive rules used by the build system.

## Implementation Notes

- [compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
  Front-end and back-end flow, data passed between stages, and where to read next.
- [error-system.md](/home/matt/nerd/docs/error-system.md)
  Structured error construction, render modes, and test coverage.
- [type-system.md](/home/matt/nerd/docs/type-system.md)
  How semantic types are represented and inferred today.
- [string-runtime.md](/home/matt/nerd/docs/string-runtime.md)
  Interpolated-string lowering, HIR/LLVM string ops, and runtime helper model.
- [stdlib.md](/home/matt/nerd/docs/stdlib.md)
  Working notes for the evolving standard library surface.
- [lsp.md](/home/matt/nerd/docs/lsp.md)
  Document lifecycle, analysis flow, and editor-facing features.
- [editor-support.md](/home/matt/nerd/docs/editor-support.md)
  VS Code and Neovim/LazyVim integration ownership and verification checklist.
- [testing.md](/home/matt/nerd/docs/testing.md)
  Test file formats, runner behaviour, and practical workflow.

## Language Design Notes

The learner-facing manual lives under:

- [manual/README.md](/home/matt/nerd/docs/manual/README.md)

The implementation-derived language specs live under:

- [spec/README.md](/home/matt/nerd/docs/spec/README.md)

The manual planning document is retained as an authoring map:

- [manual.md](/home/matt/nerd/docs/manual.md)

Use the manual appendix for source-level syntax reference:

- [manual/appendix-a-syntax-reference.md](/home/matt/nerd/docs/manual/appendix-a-syntax-reference.md)

Older language-design notes should not be added here unless they are still
actively maintained. Retain outdated planning material only when the roadmap
explicitly keeps it visible, such as [../TODO.md](/home/matt/nerd/TODO.md).
