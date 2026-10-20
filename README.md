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
10. [Optionals, Results and Error Handling](docs/learn/10-Optionals-Results-and-Error-Handling.md)
11. [Methods and Traits](docs/learn/11-Methods-and-Traits.md)

## Packaging

Run `just package` to build the release compiler and VS Code extension from
`~/nerd` and create `_dist/nerd.zip`. The archive contains these entries directly
at its root:

```text
nerd.exe
nerd-language-<version>.vsix
llvm-tools.json
mods/
examples/
docs/
```

`mods` and `examples` come from `~/nerd`; `docs` comes from this repository.
The VSIX filename uses the extension's name and version from its `package.json`.
The LLVM Scoop manifest comes from `~/nerd/etc/llvm-tools.json`.
The build uses Nerd's `just build-release nerd` and `just package` workflows,
requiring the usual release-build dependencies plus Node.js and npm.

After extracting the archive, open VS Code's Extensions view, choose
**Install from VSIX...** from its menu, and select the included `.vsix` file.

With Scoop installed, run this from the extracted directory to install the
LLVM tools described by the included manifest:

```powershell
scoop install .\llvm-tools.json
```

`test.n` contains an additional Nerd example. Directories beginning with `_`
hold generated output or local verification files and are ignored by Git.
