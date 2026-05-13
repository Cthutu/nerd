# Appendix B: Language Reference

[Manual Index](README.md) | Previous: [Syntax Reference](appendix-a-syntax-reference.md) | Next: none

This appendix summarises rules that are useful to check quickly.

## Entry Point

`main` is the program entry point.

```nerd
main :: fn () {
}
```

`main` may return `void` or an integer result, depending on the program's
intended exit code.

## Names And Scope

- Constant bindings use `::`.
- Mutable inferred bindings use `:=`.
- Local scopes are created by blocks.
- `pub` exports a module-level declaration, including mutable module bindings.
- Public plex and union fields must not name private type declarations.
- `use path` imports public names.
- `name :: use path` binds a module for qualified access.
- `pub use path` imports public names and re-exports them from this module.
- A module path first resolves to `path.n`, then falls back to `path/mod.n`.
- In a folder module, immediate sibling `.n` files are parts of the same module
  scope.

## Values

- `::` creates a constant binding.
- `=` assigns to an existing mutable target.
- `undefined` is intentionally uninitialised and must be assigned before it is
  read.
- `nil` is available for pointer-like and slice-like values where supported.
- Pointer equality supports matching pointer types, `nil`, and `^void` compared
  with any pointer type.
- Local variables, parameters, and pattern binders must be read unless their
  names start with `_`.

## Functions

- Function parameters are written as `name: Type`.
- Default parameters are written as `name: Type = expr`.
- Defaulted parameters must be trailing parameters.
- Default expressions are evaluated at the call site.
- A default expression can reference earlier parameters, but not itself or later
  parameters.
- Function types do not include defaults. A direct function-value alias can use
  defaults when it still resolves to a known defaulted function definition;
  otherwise, calling through a function value requires every argument.
- FFI declarations cannot have default parameters.
- Generic functions are written as `fn [T] (...) -> R`.
- Generic calls can infer all type arguments, as in `id(1)`.
- Generic calls can provide all type arguments, as in `id[i32](1)`.
- Concrete generic function values are written as `name[T]`.
- Partial explicit generic argument lists are invalid.

## Type Conversion

- Use `.as(Type)` for explicit casts.
- Use `p.as([]T, count)` to make a slice view from a pointer and count.
- Untyped integer constants may be used as pointer address constants when the
  destination pointer type is explicit.
- Casts do not imply ownership transfer.
- Use `.size` for the runtime byte size of a type or value. It returns `usize`.
- Strings, slices, and dynamic arrays report the size of their value header;
  use `.count` for element counts.

## Control Flow

- `return` exits the current function.
- `assert` checks a `bool` condition at runtime and exits with status 127 if it
  is false.
- `break` exits a loop or expression block.
- `continue` resumes a loop.
- Labels make nested targets explicit.
- `defer` runs code when the current scope exits.
- Deferred statements run in last-in, first-out order.

## Source Tests

- Source tests are written as `test "name" { ... }`.
- Tests are top-level declarations.
- Test names are string literals.
- Test declarations are not exported module API.
- `nerd test root.n --list` lists discovered tests.
- `nerd test root.n --filter text` runs matching tests.
- `nerd test root.n -v` prints one result line per selected test.
- The current runner uses fail-fast `assert` behaviour.

## `on`

- Short form: `on condition => expr else expr`.
- Scrutinee form: `on value { pattern => expr }`.
- Condition-chain form: `on { condition => expr }`.
- Value-producing forms must be exhaustive.
- Statement forms may be partial.
- Pattern binders use `as name`.
- Pattern guards use `on condition`.

## Loops

- `for { ... }` loops forever until broken.
- `for condition { ... }` loops while the condition is true.
- `for init; condition; update { ... }` supports C-style iteration.
- `for item in collection { ... }` iterates collection values.
- `for ^item in collection { ... }` iterates mutable item pointers where valid.
- `for index, item in collection { ... }` also binds the current index as
  `usize`.

## Data

- Tuples are positional.
- Plexes are named-field product types.
- Plexes, raw unions, and enums may be declared locally inside blocks and are
  scoped like local bindings.
- Generic plexes are written as `plex [T] { ... }` and used as `Name[T]`.
- Plexes may refer to themselves through pointer fields, such as `next ^Node`;
  direct by-value self-recursion is invalid.
- Top-level pointer aliases into a top-level collection may be used by that
  collection's initializer.
- Raw unions overlap storage and are low-level.
- Generic raw unions are written as `union [T] { ... }`.
- Enums are tagged variants.
- Generic enums are written as `enum [T] { ... }`.
- Inherent methods are grouped with `impl Type { ... }`.
- The first method parameter is the receiver. A receiver of type `^T` lets
  `value.method(...)` mutate `value`.
- Associated functions in impl blocks are called as `Type.name(...)` and return
  `Self` or `^Self`.
- Public methods inside an impl block are imported with their type's module.
- Fixed arrays own their elements and carry length in the type.
- Slices borrow contiguous storage.
- Dynamic arrays own growable storage and should be freed when no longer used.

## Generics

- The current generic system supports type parameters only.
- Numeric or value generic parameters are future work.
- Constraints are future work and are expected to build on traits.
- Every explicit generic use must provide all type arguments.
- Generic instantiations are compiled as concrete functions and types.

## FFI

- `ffi "lib" name (...)` exposes the same Nerd and foreign name.
- `ffi "lib" { name (...) }` groups declarations that use the same library.
- `ffi "lib" { local :: foreign (...) }` renames one grouped declaration.
- `local :: ffi "lib" foreign (...)` uses `local` in Nerd and `foreign` for
  linking.
- C strings use `c"..."`.
- Prefer wrappers around raw FFI declarations.
- Keep ownership explicit at the boundary.
