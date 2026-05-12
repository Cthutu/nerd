# Nerd Manual

This manual teaches the Nerd language from first principles. It focuses on
source-level behaviour: how to write programs, how values and types work, and
how the main language constructs fit together.

Compiler internals such as HIR, LLVM lowering, installed smoke tests, and editor
integration checks are documented outside the manual in the developer
documentation. Start at `../README.md` for those implementation notes.

The examples are intentionally small and executable. Standard library functions
are used where they keep examples readable, but the standard library itself is
documented separately because it is still evolving.

## Reading Order

1. [First Programs](part01-first-programs.md)
2. [Values, Bindings, And Assignment](part02-values-bindings-and-assignment.md)
3. [Primitive Types And Expressions](part03-primitive-types-and-expressions.md)
4. [Functions](part04-functions.md)
5. [Blocks, Scope, And Cleanup](part05-blocks-scope-and-cleanup.md)
6. [Branching With `on`](part06-branching-with-on.md)
7. [Loops](part07-loops.md)
8. [Compound Data](part08-compound-data.md)
9. [Dynamic Arrays And Manual Memory](part09-dynamic-arrays-and-manual-memory.md)
10. [Modules](part10-modules.md)
11. [Interoperability With C](part11-interoperability-with-c.md)
12. [Building A Small Program](part12-building-a-small-program.md)
13. [Diagnostics And Debugging](part13-diagnostics-and-debugging.md)

## Appendices

- [Syntax Reference](appendix-a-syntax-reference.md)
- [Language Reference](appendix-b-language-reference.md)
