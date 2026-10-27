# Appendix A: Syntax and Pragmas

[Previous: Building and Configuring Programs](23-Building-and-Configuring-Programs.md)

[Guide index](../../README.md)

This appendix is a compact index of the syntax taught in the guide. Names such
as `Type`, `value`, and `condition` below are placeholders, rather than complete
programs. Follow the chapter links for runnable examples and semantic rules.

## Bindings, storage, and functions

| Form | Meaning | Chapter |
| --- | --- | --- |
| `name :: value`, `name: Type : value` | Immutable binding. | [2](02-First-Programs.md) |
| `name := value`, `name: Type = value` | Mutable variable. | [4](04-Variables-and-Scope.md) |
| `name: Type`, `name: Type = undefined` | Default initialisation or explicit opt-out. | [4](04-Variables-and-Scope.md) |
| `fn (arg: Type) -> Type { ... }` | Block function. | [2](02-First-Programs.md) |
| `fn (arg: Type) => expression` | Expression function. | [2](02-First-Programs.md) |
| `fn (arg: Type = value)`; `call(arg = value)` | Default and named override. | [16](16-Special-Function-Declarations.md) |
| `fn [T](arg: T)`; `call[i32](value)` | Generic declaration and explicit application. | [12](12-Generics.md) |
| `fn (arg :: Type)` | Compile-time value parameter. | [20](20-Compile-Time-Parameters-and-Macros.md) |
| `fn { member_a member_b }` | Multi-function declaration; members normally occupy separate lines. | [16](16-Special-Function-Declarations.md) |
| `fn (...) -> !` | Function that never returns. | [4](04-Variables-and-Scope.md) |
| `value.as(Type)`, `value.as()` | Explicit conversion, with a named or contextual destination. | [3](03-Primitive-Types-and-Expressions.md) |

## Data and ownership

| Form | Meaning | Chapter |
| --- | --- | --- |
| `plex { field Type }`, `value with { field: expression }` | Record and updated copy. | [5](05-Plexes-Unions-Enums-and-Tuples.md) |
| `plex #c { ... }`, `plex #packed { ... }` | Explicit layout choices. | [5](05-Plexes-Unions-Enums-and-Tuples.md) |
| `union { field Type }`, `enum { Variant Variant(Type) }` | Shared storage or tagged alternatives. | [5](05-Plexes-Unions-Enums-and-Tuples.md) |
| `union { plex { x i32 y i32 } bits u64 }` | Anonymous record members with promoted fields. | [5](05-Plexes-Unions-Enums-and-Tuples.md) |
| `(left, right) := pair`, `{ x, y } := point` | Destructuring. | [5](05-Plexes-Unions-Enums-and-Tuples.md) |
| `[N]T`, `[]T`, `[..]T` | Fixed array, borrowed slice, dynamic array. | [6](06-Arrays-Slices-and-Dynamic-Arrays.md) |
| `values[start..end]`, `values[..]` | Borrowed slice with exclusive end. | [6](06-Arrays-Slices-and-Dynamic-Arrays.md) |
| `values[start..end] = source` | Fill with one element or copy an equal-length slice; overlap is supported. | [6](06-Arrays-Slices-and-Dynamic-Arrays.md) |
| `copy: [..]T = array_or_slice` | Fresh owned buffer containing a shallow element copy. | [6](06-Arrays-Slices-and-Dynamic-Arrays.md) |
| `^T`, `^value`, `pointer^` | Pointer type, address, dereference. | [7](07-Pointers.md) |
| `pointer.as([]T, count)` | Borrowed pointer/count view. | [7](07-Pointers.md) |
| `view: []T = pointer.as(count)`, `view: []T = array.as()` | Contextual slice casts; pointers require a count. | [7](07-Pointers.md), [6](06-Arrays-Slices-and-Dynamic-Arrays.md) |
| `box[T]()`, `box[T](count)`, `box[T](ptr)` | Allocate or adopt owned storage. | [18](18-Boxes-and-Ownership.md) |
| `atomic[T]` | Atomic scalar storage. | [21](21-Atomic-Values.md) |
| `?T`, `T\E`, `value!`, `expression?` | Optional, result, error injection, propagation. | [10](10-Optionals-Results-and-Error-Handling.md) |
| `expression \ fallback`, `expression else error { ... }` | Default extraction or handler before propagation. | [10](10-Optionals-Results-and-Error-Handling.md) |

## Control flow, modules, and compilation

| Form | Meaning | Chapter |
| --- | --- | --- |
| `on condition => expression else expression` | Conditional expression. | [9](09-Branching-and-Pattern-Matching.md) |
| `on subject { pattern => expression }` | Pattern matching. | [9](09-Branching-and-Pattern-Matching.md) |
| `for condition { ... }`, `for item in values { ... }` | Condition or iteration loop. | [11](11-Loops.md) |
| `break`, `again`, `return` | Exit a loop/block, repeat a loop, or exit a function. | [11](11-Loops.md), [4](04-Variables-and-Scope.md) |
| `defer statement`, `undo statement` | Scope-exit or failure-only cleanup. | [10](10-Optionals-Results-and-Error-Handling.md) |
| `impl Type { ... }`, `impl Trait for Type { ... }` | Inherent methods or trait implementation. | [13](13-Methods-and-Traits.md) |
| `show :: fn (self: ^Self) -> string` | Required borrowed receiver for core `Display`. | [13](13-Methods-and-Traits.md) |
| `$"{value:02X}"` | Integer interpolation with minimum width and base. | [14](14-String-Interpolation.md) |
| `name :: use module`, `use module` | Qualified or direct imports. | [15](15-Modules.md) |
| `use parent { child_a child_b }` | Grouped module imports. | [15](15-Modules.md) |
| `pub name :: value` | Public declaration or re-export. | [15](15-Modules.md) |
| `ffi "library" symbol (arg: Type) -> Type` | Foreign function declaration. | [16](16-Special-Function-Declarations.md) |
| `fn (fixed: Type, args: ...)` | Receiving C variadic definition. | [22](22-Advanced-C-Interoperability.md) |
| `on "define" { ... }`, `assert on "define"` | Compile-time selection or requirement. | [17](17-Defines.md) |
| `assert condition, "message"` | Runtime assertion; message optional. | [19](19-Assertions-and-Testing.md) |
| `test "name" { ... }`, `test { ... }` | Named test or test-only declarations. | [19](19-Assertions-and-Testing.md) |
| `@file`, `@line`, `@embed("path")` | Built-in macro expressions. | [20](20-Compile-Time-Parameters-and-Macros.md) |
| `build { define: feature }` | Source build settings. | [23](23-Building-and-Configuring-Programs.md) |

## Pragmas

A pragma carries compiler metadata. The general syntax is:

```text
pragma name
pragma name(123, 1.5, "text", yes, no)
```

Parameters are literal integers, floats, strings, or booleans. The parser
accepts unknown pragma names and ignores them; acceptance does not establish
that an option has taken effect. The recognised uses relevant to this guide
are:

| Pragma | Contract |
| --- | --- |
| `pragma windowed` | Legacy module-local equivalent of `build { windowed: yes }`. Prefer the build block. |
| `pragma compiler_intrinsic("atomic.load")` and the other `atomic.*` identities | Identify compiler-supported methods in an `impl` for `atomic[T]`. The compiler validates the operation and signature. These are standard-library implementation metadata. |

The atomic identities are `atomic.load`, `atomic.store`, `atomic.exchange`,
`atomic.fetch_add`, `atomic.fetch_sub`, `atomic.fetch_and`, `atomic.fetch_or`,
`atomic.fetch_xor`, `atomic.compare_exchange`, and
`atomic.compare_exchange_weak`. Application code should import `std.atomics`
and call its methods rather than redeclaring those operations. A matching
method name alone does not make an ordinary method an atomic intrinsic.

An `intrinsic "name" (...)` binding is a separate declaration form, described
in Chapter 22. A pragma does not provide a general annotation system or a
portable way to enable arbitrary backend operations.
