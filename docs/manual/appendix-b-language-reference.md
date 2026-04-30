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
- `pub` exports a module-level declaration.
- `use` imports public names.
- `mod` binds a module for qualified access.

## Values

- `::` creates a constant binding.
- `=` assigns to an existing mutable target.
- `undefined` is intentionally uninitialised and should be assigned before use.
- `nil` is available for pointer-like and slice-like values where supported.

## Type Conversion

- Use `.as(Type)` for explicit casts.
- Use `p.as([]T, count)` to make a slice view from a pointer and count.
- Casts do not imply ownership transfer.
- Use `.size` for the runtime byte size of a type or value. It returns `usize`.
- Strings, slices, and dynamic arrays report the size of their value header;
  use `.count` for element counts.

## Control Flow

- `return` exits the current function.
- `assert` checks a `bool` condition at runtime and stops the program if it is
  false.
- `break` exits a loop or expression block.
- `continue` resumes a loop.
- Labels make nested targets explicit.
- `defer` runs code when the current scope exits.
- Deferred statements run in last-in, first-out order.

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

## Data

- Tuples are positional.
- Plexes are named-field product types.
- Raw unions overlap storage and are low-level.
- Enums are tagged variants.
- Fixed arrays own their elements and carry length in the type.
- Slices borrow contiguous storage.
- Dynamic arrays own growable storage and should be freed when no longer used.

## FFI

- `ffi "lib" name (...)` exposes the same Nerd and foreign name.
- `local :: ffi "lib" foreign (...)` uses `local` in Nerd and `foreign` for
  linking.
- C strings use `c"..."`.
- Prefer wrappers around raw FFI declarations.
- Keep ownership explicit at the boundary.
