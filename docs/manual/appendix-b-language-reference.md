# Appendix B: Language Reference

[Manual Index](README.md) | Previous: [Syntax Reference](appendix-a-syntax-reference.md) | Next: none

This appendix summarises rules that are useful to check quickly.

## Entry Point

`main` is the program entry point.

```nerd
main :: fn () {
}
```

It may also take the command-line argument slice:

```nerd
main :: fn (args: []string) {
}
```

`args[0]` is the executable path supplied by the operating system. User
arguments passed after `nerd run source.n --` start at `args[1]`.

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
- A `mod.n` fallback forms a package boundary: external imports cannot continue
  below that path, but `mod.n` can import child files and re-export selected
  names.
- In a folder module, immediate sibling `.n` files are parts of the same module
  scope unless `mod.n` explicitly imports them as child modules.

## Values

- `::` creates a constant binding.
- `=` assigns to an existing mutable target.
- `undefined` is intentionally uninitialised and must be assigned before it is
  read.
- A local `name: Type` variable calls `core.Default[Type].default()` when a
  concrete implementation exists; otherwise it uses the type's default storage
  value.
- `name: Type = undefined` opts out of both `Default` and default storage
  initialisation.
- `nil` is available for pointer-like and slice-like values where supported.
- Slices are borrowing views. A `[]T` carries data and count, but it does not
  own or free the referenced storage.
- `box[T]` owns one runtime heap allocation for a `T`, or is nil.
- `box[T]()` allocates one default-initialised `T`; `box[T](ptr)` adopts a
  runtime-heap-compatible `^T`.
- `box[T]` moves on assignment, box-parameter calls, and return; the source
  box is set to nil.
- `box[T]` borrows implicitly as `^T` for function calls and dot access.
- `box[T]` converts implicitly to `bool`, where nil is `no` and non-nil is
  `yes`.
- `box.free()` releases the allocation and resets the box to nil. Local and
  parameter boxes that still own an allocation are freed automatically on scope
  exit.
- Pointer equality supports matching pointer types, `nil`, and `^void` compared
  with any pointer type.
- Non-built-in equality with `==` and `!=` uses the canonical `core.Eq`
  implementation for the value type.
- Non-built-in ordering with `<`, `<=`, `>`, and `>=` uses the canonical
  `core.Order` implementation for the value type.
- Local variables, parameters, and pattern binders must be read unless their
  names start with `_`.

## Functions

- Function parameters are written as `name: Type`.
- Function parameter bindings are immutable. Assigning to a parameter, or to
  storage directly contained in a by-value parameter, is rejected.
- Default parameters are written as `name: Type = expr`.
- Defaulted parameters must be trailing parameters.
- Default expressions are evaluated at the call site.
- `@file` and `@line` inside default parameter expressions therefore expand to
  the caller's source filename and line number.
- A default expression can reference earlier parameters, but not itself or later
  parameters.
- Function types do not include defaults. A direct function-value alias can use
  defaults when it still resolves to a known defaulted function definition;
  otherwise, calling through a function value requires every argument.
- FFI declarations cannot have default parameters.
- Generic functions are written as `fn [T] (...) -> R`.
- Generic calls can infer all type arguments, as in `id(1)`.
- Generic calls can provide all type arguments, as in `id[i32](1)`.
- Generic method calls can provide type arguments after the method name, as in
  `arena.alloc[i32]()`.
- A trait implementation is atomic: all required members for one trait/type
  pair must appear in the same `impl Trait for Type` block. The language does
  not merge partial implementations of the same trait for the same type across
  multiple impl blocks.
- Duplicate concrete trait implementations and overlapping generic trait
  implementations are rejected.
- Concrete generic function values are written as `name[T]`.
- Partial explicit generic argument lists are invalid.

## Type Conversion

- Use `.as(Type)` for explicit casts.
- Use `p.as([]T, count)` to make a slice view from a pointer and count.
- Untyped integer address constants must use explicit pointer casts such as
  `n.as(^T)`.
- `usize` and `isize` values may be explicitly cast to pointer types at FFI or
  platform boundaries.
- Casts do not imply ownership transfer.
- Use `.size` for the runtime byte size of a type or value. It returns `usize`.
- Strings, slices, and dynamic arrays report the size of their value header.
  Fixed arrays, strings, slices, and dynamic arrays expose `.count` for element
  counts.
- Dot access automatically dereferences pointers when the pointee provides the
  requested tuple, collection, plex, or union member.
- `@file` expands to the current source filename as a `string`.
- `@line` expands to the current 1-based source line as an untyped integer.
- `@embed("path")` embeds a source-relative file as static binary data and
  expands to a `[]u8` slice backed by that data.

## Control Flow

- All loops use `for`.
- All branch forms use `on`.
- `return` exits the current function.
- `assert` checks a `bool` condition at runtime and exits with status 127 if it
  is false.
- `break` exits a loop or expression block.
- `again` resumes a loop.
- Labels make nested targets explicit.
- `defer` runs code when the current scope exits.
- Deferred statements run in last-in, first-out order.

## Source Tests

- Source tests are written as `test "name" { ... }`.
- Test-only declarations are written as `test { ... }`.
- Tests are top-level declarations.
- Test names are string literals.
- Test declarations are not exported module API.
- Declarations inside `test { ... }` are ignored by normal builds and compiled
  only by `nerd test`.
- `nerd check root.n` checks lexing, parsing, imports, and semantic analysis
  without generating HIR, LLVM, or an executable.
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
- Bare identifiers in patterns bind by default. Use `for name` to compare with
  an existing runtime value.
- Pattern guards use `on condition`.

## Loops

- `for { ... }` loops forever until broken.
- `for condition { ... }` loops while the condition is true.
- `for init; condition; update { ... }` supports C-style iteration.
- `for item in collection { ... }` iterates built-in collections and
  `Iterator[Item]` values.
- Built-in collection iteration binds pointers to elements; dereference
  explicitly with `item^` when the pointed-to value is needed.
- `for item in [start..end] { ... }` iterates integer values from `start` up to
  but not including `end`.
- `for item in [start..=end] { ... }` iterates integer values from `start`
  through `end`.
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
- `...` at the end of a plex literal initialises omitted fields with their
  default values.
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
- Trait declarations use `Name :: trait { member :: fn (Self) -> Type }`.
  `Name :: trait [T] { member :: fn (Self) -> T }` declares generic trait
  parameters. Generic trait declarations parse and format, but generic trait
  implementations are not semantic yet.
  `Name :: trait for Value { ... }` names the trait self type `Value` instead
  of `Self`.
- Trait implementations use `impl TraitName for Type { ... }` and must provide
  all required member names. Methods supplied by a trait impl can be called on
  the implemented type with normal receiver syntax, such as `value.member()`.
  Inherent impl methods take precedence over trait impl methods. Receiver calls
  are ambiguous when multiple trait impl methods with the same name are valid
  for the receiver type. Use `Trait.member(value, ...)` to explicitly select a
  trait member implementation. Use `Trait[Type].member(...)` when there is no
  receiver argument to infer the implementation type. Trait members are not
  ordinary top-level functions, so a bare call such as `member(value)` does not
  resolve to a trait member.
  Local trait implementations are checked against the required member
  signatures. Duplicate concrete implementations and overlapping generic
  implementations for the same trait are rejected. Generic trait
  implementation parameters are inferred from the implementation target, as in
  `impl Display for Box[T] where T: Display { ... }`.
- Fixed arrays own their elements and carry length in the type.
- Slices borrow contiguous storage.
- Dynamic arrays own growable storage and should be freed when no longer used.
- `arena` is an opaque built-in type that reserves one 4 GiB address range,
  grows by committing pages inside that range, and keeps allocation addresses
  stable while it grows.
- Copying an `arena` value copies a handle to the same underlying allocation
  state; it does not copy the arena's storage.
- Arena marks are `u32` cursor values. Restoring to a mark invalidates later
  allocations; resetting an arena invalidates all allocations from it.
- Runtime interpolated strings are allocated from the temporary arena and remain
  valid until `temp_arena.reset()`.
- Interpolation uses built-in formatting for primitive and built-in aggregate
  values. For other values, the type must implement the canonical
  `core.Display` trait; interpolation calls `Display.show(value)`.

## Generics

- The current generic system supports type parameters only.
- Numeric or value generic parameters are not part of the current generic
  syntax.
- Generic functions and generic impl blocks may use `where T: Trait` clauses.
  Multiple constraints are comma-separated. Constraint trait names are checked,
  and each concrete instantiation must have matching trait implementations.
- Every explicit generic use must provide all type arguments.
- Generic instantiations are compiled as concrete functions and types.

## FFI

- `ffi "lib" name (...)` exposes the same Nerd and foreign name.
- `ffi "lib" { name (...) }` groups declarations that use the same library.
- `ffi "lib" { local :: foreign (...) }` renames one grouped declaration.
- `local :: ffi "lib" foreign (...)` uses `local` in Nerd and `foreign` for
  linking.
- C strings use `c"..."`.
- Multi-line strings use `"""..."""`; a leading newline is dropped and closing
  delimiter indentation is trimmed from body lines.
- Prefer wrappers around raw FFI declarations.
- Keep ownership explicit at the boundary.
