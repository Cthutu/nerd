# Nerd documentation

Tutorials for the [Nerd programming language](https://github.com/Cthutu/nerd).
This guide assumes familiarity with C and introduces Nerd's syntax and behaviour
from the beginning.

## Chapters

1. [Overview](docs/learn/01-Overview.md)
2. [First Programs and Bindings](docs/learn/02-First-Programs.md)
3. [Primitive Types and Expressions](docs/learn/03-Primitive-Types-and-Expressions.md)
4. [Variables and Scope](docs/learn/04-Variables-and-Scope.md)
5. [Plexes, Unions, Enums and Tuples](docs/learn/05-Plexes-Unions-Enums-and-Tuples.md)
6. [Arrays, Slices and Dynamic Arrays](docs/learn/06-Arrays-Slices-and-Dynamic-Arrays.md)
7. [Pointers](docs/learn/07-Pointers.md)
8. [Arenas](docs/learn/08-Arenas.md)
9. [Branching and Pattern Matching](docs/learn/09-Branching-and-Pattern-Matching.md)

## Packaging

Run `just package` to build the release compiler from `~/nerd` and create
`_dist/nerd.zip`. The archive contains these entries directly at its root:

```text
nerd.exe
mods/
examples/
docs/
```

`mods` and `examples` come from `~/nerd`; `docs` comes from this repository.
The build uses Nerd's `just build-release nerd` workflow and requires its
usual release-build dependencies.

`test.n` contains an additional Nerd example. Directories beginning with `_`
hold generated output or local verification files and are ignored by Git.
