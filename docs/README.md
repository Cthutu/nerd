# Developer Documentation

This folder contains developer-facing documentation for the repository.

## Reading Order

If you are new to the codebase, read these first:

1. [overviews/INTERNALS.md](/home/matt/nerd/docs/overviews/INTERNALS.md)
2. [compiler-pipeline.md](/home/matt/nerd/docs/compiler-pipeline.md)
3. [error-system.md](/home/matt/nerd/docs/error-system.md)
4. [type-system.md](/home/matt/nerd/docs/type-system.md)
5. [string-runtime.md](/home/matt/nerd/docs/string-runtime.md)
6. [lsp.md](/home/matt/nerd/docs/lsp.md)
7. [testing.md](/home/matt/nerd/docs/testing.md)

## Overviews

- [overviews/INTERNALS.md](/home/matt/nerd/docs/overviews/INTERNALS.md)
  High-level architecture and subsystem map.
- [overviews/FORMAT.md](/home/matt/nerd/docs/overviews/FORMAT.md)
  Current formatter rules and behaviour.
- [overviews/SYNTAX.md](/home/matt/nerd/docs/overviews/SYNTAX.md)
  Short syntax reference for the currently implemented language surface.
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
  Interpolated-string lowering, IR string ops, and runtime helper model.
- [lsp.md](/home/matt/nerd/docs/lsp.md)
  Document lifecycle, analysis flow, and editor-facing features.
- [testing.md](/home/matt/nerd/docs/testing.md)
  Test file formats, runner behaviour, and practical workflow.

## Language Design Notes

The existing `docs/Nerd` folder contains older language-design material and
reference notes:

- [Nerd/Overview.md](/home/matt/nerd/docs/Nerd/Overview.md)
- [Nerd/Specifications.md](/home/matt/nerd/docs/Nerd/Specifications.md)
- [Nerd/Grammar.md](/home/matt/nerd/docs/Nerd/Grammar.md)
