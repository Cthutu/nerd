# Developer Documentation

This folder contains developer-facing documentation for the repository.

## Source Of Truth

Use [../ROADMAP.md](../ROADMAP.md) for current project direction,
active work, and cross-cutting engineering rules. Use the learner-facing
[manual/README.md](manual/README.md) for source-level
language behaviour and [spec/README.md](spec/README.md)
for implementation-derived language facts.

The other documents in this folder are implementation notes. Keep them when
they describe compiler structure, subsystem ownership, diagnostics, tooling, or
standard-library surface that is not already covered by the manual or specs.
When one of these notes disagrees with the roadmap, manual, specs, tests, or
implementation, verify against the implementation and update the note before
relying on it.

## Reading Order

If you are new to the codebase, read these first:

1. [overviews/INTERNALS.md](overviews/INTERNALS.md)
2. [compiler-pipeline.md](compiler-pipeline.md)
3. [error-system.md](error-system.md)
4. [type-system.md](type-system.md)
5. [string-runtime.md](string-runtime.md)
6. [stdlib.md](stdlib.md)
7. [lsp.md](lsp.md)
8. [editor-support.md](editor-support.md)
9. [configuration.md](configuration.md)
10. [testing.md](testing.md)

If you are learning the Nerd language rather than the compiler implementation,
start with [manual/README.md](manual/README.md). If you are
validating the language against the implementation, use
[spec/README.md](spec/README.md).

## Overviews

- [overviews/INTERNALS.md](overviews/INTERNALS.md)
  High-level architecture and subsystem map.
- [overviews/FORMAT.md](overviews/FORMAT.md)
  Current formatter rules and behaviour.
- [overviews/build-directives.md](overviews/build-directives.md)
  Build/module directive rules used by the build system.

## Implementation Notes

- [compiler-pipeline.md](compiler-pipeline.md)
  Front-end and back-end flow, data passed between stages, and where to read next.
- [error-system.md](error-system.md)
  Structured error construction, render modes, and test coverage.
- [type-system.md](type-system.md)
  How semantic types are represented and inferred today.
- [string-runtime.md](string-runtime.md)
  Interpolated-string lowering, HIR/LLVM string ops, and runtime helper model.
- [stdlib.md](stdlib.md)
  Working notes for the evolving standard library surface.
- [stdlib-thread-sync.md](stdlib-thread-sync.md)
  Thread, mutex and condition-variable lifetime contracts.
- [stdlib-raptor.md](stdlib-raptor.md)
  Typed tasks, worker queues and explicitly pumped main-thread queues.
- [std-network-foundation.md](std-network-foundation.md)
  Raw sockets, readiness and deadline contracts.
- [stdlib-nexus.md](stdlib-nexus.md)
  Socket-associated messages, TCP/UDP, request/reply and Telnet.
- [stdlib-source-inventory.md](stdlib-source-inventory.md)
  Pinned upstream APIs, tests and provenance for the library ports.
- [tutorials/README.md](tutorials/README.md)
  Runnable Raptor and Nexus API walkthroughs and remaining tutorial work.
- [lsp.md](lsp.md)
  Document lifecycle, analysis flow, and editor-facing features.
- [editor-support.md](editor-support.md)
  VS Code and Neovim/LazyVim integration ownership and verification checklist.
- [configuration.md](configuration.md)
  Project-level environment and compile-time define configuration.
- [testing.md](testing.md)
  Test file formats, runner behaviour, and practical workflow.

## Language Design Notes

The learner-facing manual lives under:

- [manual/README.md](manual/README.md)

The implementation-derived language specs live under:

- [spec/README.md](spec/README.md)

Manual authoring guidance lives alongside the manual:

- [manual/MANUAL-STYLE.md](manual/MANUAL-STYLE.md)

Use the manual appendix for source-level syntax reference:

- [manual/appendix-a-syntax-reference.md](manual/appendix-a-syntax-reference.md)

Older language-design notes should not be added here unless they are still
actively maintained. Retain outdated planning material only when the roadmap
explicitly keeps it visible.

- [LLVM toolchain and doctor](toolchain.md): native build dependencies, platform
  linking configuration, and dependency diagnostics.
- [Compiler profiling](compiler-profiling.md): per-module CPU/wall/allocation
  records and a repeatable single-core benchmark matrix.

## Maintenance

Keep current behaviour in the manual, specs and subsystem docs, and open work in
[the roadmap](../ROADMAP.md) or the [library plan](stdlib-expansion-plan.md).
Completed audits, one-off repair reports and superseded handoffs belong in Git
history. Keep reproducible validation tools, source provenance, accepted decision
records and dated measurements when they still support a maintained contract.
