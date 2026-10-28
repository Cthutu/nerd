# Nerd documentation

Tutorials for the [Nerd programming language](https://github.com/Cthutu/nerd).
This guide assumes familiarity with C and introduces Nerd's syntax and behaviour
from the beginning.

## Chapters

1. [Overview](01-Overview.md)
2. [First Programs and Bindings](02-First-Programs.md)
3. [Primitive Types and Expressions](03-Primitive-Types-and-Expressions.md)
4. [Variables and Scope](04-Variables-and-Scope.md)
5. [Plexes, Unions, Enums and Tuples](05-Plexes-Unions-Enums-and-Tuples.md)
6. [Arrays, Slices and Dynamic Arrays](06-Arrays-Slices-and-Dynamic-Arrays.md)
7. [Pointers](07-Pointers.md)
8. [Arenas](08-Arenas.md)
9. [Branching and Pattern Matching](09-Branching-and-Pattern-Matching.md)
10. [Optionals, Results and Error Handling](10-Optionals-Results-and-Error-Handling.md)
11. [Loops with `for`](11-Loops.md)
12. [Generics](12-Generics.md)
13. [Methods and Traits](13-Methods-and-Traits.md)
14. [String Interpolation](14-String-Interpolation.md)
15. [Modules](15-Modules.md)
16. [Special Function Declarations](16-Special-Function-Declarations.md)
17. [Defines](17-Defines.md)
18. [Boxes and Ownership](18-Boxes-and-Ownership.md)
19. [Assertions and Testing](19-Assertions-and-Testing.md)
20. [Compile-Time Parameters and Built-In Macros](20-Compile-Time-Parameters-and-Macros.md)
21. [Atomic Values](21-Atomic-Values.md)
22. [Advanced C Interoperability](22-Advanced-C-Interoperability.md)
23. [Building and Configuring Programs](23-Building-and-Configuring-Programs.md)

[Appendix A: Syntax and Pragmas](Appendix-A-Syntax-and-Pragmas.md)

## Further learning

- [Building a small program](Building-a-Small-Program.md)
- [Understanding diagnostics](Diagnostics-and-Debugging.md)
- [Standard-library tutorials](../tutorials/README.md)
- [`std.term`](../stdlib-term.md)
- [Toolchain setup](../toolchain.md) and [debugger support](../../DEBUGGER.md)

## Distribution

Run `just dist` from the repository root to build `_dist/nerd.zip` containing
this checkout's release compiler, learning guide, standard library, LLVM Scoop
manifest and examples. `just package` builds the VS Code extension separately.
See [distribution packaging](../distribution.md).
