# Type System

This document describes the current implementation of type information in the
semantic layer for Milestone 5.

The main code is in
[src/compiler/sema/sema.h](/home/matt/nerd/src/compiler/sema/sema.h) and
[src/compiler/sema/sema.c](/home/matt/nerd/src/compiler/sema/sema.c).

## Design Goal

The current design keeps the AST compact and stores type facts in semantic side
tables instead of growing AST nodes.

That means:

- the parser recognises syntax
- semantic analysis decides what the syntax means
- type rows live in `Sema.types`
- AST nodes are linked to inferred or declared types through side tables

## Type Representation

Each semantic type is stored as a compact `SemaType` row:

- `kind`
- `param_count`
- `first_param_type`
- `return_type`

The current `SemaTypeKind` set includes:

- `void`
- `untyped integer`
- `string`
- `bool`
- integer primitives such as `i32`, `u16`, `isize`, and `usize`
- floating-point primitives `f32` and `f64`
- function types
- tuple types
- fixed array types
- slice types
- pointer types

Function types store their parameter list in the flattened
`Sema.type_param_types` side table and keep the return type in
`return_type`. The representation is intentionally table-oriented rather than
pointer-heavy.

Function values currently reuse the same canonical signature rows. Named
functions therefore have a semantic function type such as `fn (i32, i32) ->
i32`, and when they are used as runtime values the back end lowers them through
function-pointer-compatible storage in generated C.

Tuple types also use the flattened `Sema.type_param_types` side table. The
tuple row stores its element count in `param_count` and the first element type in
`first_param_type`; `return_type` is unused. Tuple syntax is `(T1, T2, ...)`,
with `(T,)` for a one-element tuple and `(T)` remaining ordinary grouped type
syntax.

Fixed array types are written `[N]T`, where `N` is a compile-time integer
length and `T` is the element type. The current implementation stores the
element type and length in the canonical semantic type row so `[2]i32` and
`[3]i32` are distinct types.

Slice types are written `[]T`. A slice is a fat pointer containing a data
pointer and a count. The element type is part of the canonical type, so `[]i32`
and `[]u8` are distinct types.

The built-in `string` type uses the same data/count representation as `[]u8`,
but it is intentionally a distinct type. A `string` carries the extra invariant
that its contents are valid UTF-8.

Pointer types are written `^T`. The pointee type is part of the canonical type,
so `^i32` and `^[3]i32` are distinct types.

At the current milestone boundary, source-level function-valued annotations also
reuse that same function type syntax:

- `adder: fn (i32, i32) -> i32 = add`

Explicit `^fn (...) -> ...` pointer syntax is deferred until the wider pointer
type system is introduced.

Function-body style is also part of the type rules:

- fat-arrow functions infer their return type from the body expression
- thin-arrow function types pair with block bodies and explicit `return`
- mixing `->` with `=>` is rejected with a dedicated semantic diagnostic

Short-form `on` expressions also participate in semantic typing:

- the condition must have type `bool`
- both branches must produce exactly the same type
- no implicit casts are inserted between branches

Expression blocks and value-producing loops use the same explicit control-flow
value model:

- a `break <expr>` targeting the block or loop supplies the result value
- all value-producing breaks for the same target must merge to one type
- a finite value-producing loop must include `else { break <expr> }` for the
  normal loop-exhaustion path
- loop `else` is only valid when the loop body has a reachable value-producing
  `break`

Infinite loop expressions do not need an `else` because they cannot exhaust
normally.

Block-form `on` currently supports:

- constant value patterns
- comma-separated constant alternatives
- integer ranges through `..` and `..=`

Range endpoints are checked semantically against the scrutinee type and must be
compile-time constants. Empty integer ranges are rejected with a dedicated
semantic error.

Source-level boolean values are also available as literal keywords:

- `yes`
- `no`

## Canonicalisation

`sema_add_type(...)` interns type rows by value. If the same type row already
exists, the semantic pass reuses its index.

This keeps type equality cheap:

- equal rows share one canonical index
- side tables can compare type indices directly in many cases

## Declaration And Node Tables

`Sema` stores several related tables:

- `decls`
  Top-level declarations.
- `locals`
  Function-local declarations.
- `types`
  Canonical semantic types.
- `node_type_indices`
  Per-AST-node inferred or declared type.
- `node_is_type_expr`
  Marks AST subtrees that should be interpreted as type syntax during resolution.

This is the main mechanism that keeps type facts out of the AST itself.

## Built-In Types

Built-in primitive types are materialised through helper functions such as
`sema_builtin_type(...)`.

They are semantic facts, not parser keywords with hard-coded AST payloads. The
parser mostly sees symbols and type syntax; semantic analysis resolves them.

## Untyped Integers

Integer literals begin life as `untyped integer`. This lets the semantic pass
delay commitment until surrounding context is known.

When a concrete runtime type is required, `sema_materialise_type(...)` currently
maps `untyped integer` to `i32`.

## Untyped Floats

Floating-point literals begin life as `untyped float`.

As with untyped integers, the semantic pass can adopt a concrete destination
type from surrounding numeric context. When no narrower type is forced,
`sema_materialise_type(...)` currently maps `untyped float` to `f64`.

## Type Aliases

Type aliases are normal top-level bindings whose right-hand side resolves to a
type:

- `Price :: u16`
- `MainFn :: fn () -> i32`

Alias classification is a semantic pass, not a parser special case. The parser
still records a normal binding shape and semantic analysis decides whether the
binding names a runtime value or a type alias.

Alias cycles are diagnosed during type classification before normal dependency
ordering. They now have a dedicated semantic error so alias cycles are kept
distinct from value dependency cycles.

## Variables And Storage

Variables are represented semantically rather than through wider AST nodes.
Current storage-eligible types are:

- concrete integer types
- `bool`
- `string`
- `f32`
- `f64`
- tuples whose elements are all storage-eligible
- fixed arrays whose element type is storage-eligible
- slices whose element type is storage-eligible
- pointers

Zero-initialised declarations such as `count: i32` and `name: string` are
checked in sema and then lowered using type-aware zero values.

## Tuples

Tuple literals use the same comma rule as tuple types:

- `(1, "one")` has type `(i32, string)` once stored
- `(value,)` is a one-element tuple
- `(value)` is just a grouped expression

Tuple fields are accessed with zero-based dot indices:

- `pair.0`
- `pair.1`

The semantic pass checks that field access is applied to a tuple and that the
index is within range. Tuple values currently lower to generated C structs with
numbered fields such as `_0` and `_1`.

## Fixed Arrays

Fixed array literals use square brackets:

- `[1, 2, 3]`
- `["red", "green"]`

If there is no expected type, sema infers the element type from the first item
and checks the remaining items against it. Untyped numeric elements materialise
as needed when an expected array type is present:

- `values: [3]i32 = [1, 2, 3]`

The array length is part of the type, so `[2]i32` and `[3]i32` do not match.
Empty array literals currently require an expected fixed-array type because
there is no element from which to infer a type.

Fixed arrays are indexed with square brackets:

- `values[0]`
- `values[i]`

The index expression must be an integer type. In debug builds, generated C emits
a bounds check before each fixed-array index and aborts with a fatal message if
the index is outside the fixed length. Release builds may omit those checks.

Fixed arrays do not implicitly coerce to slices. Use slicing syntax when a slice
view is required:

- `all: []i32 = values[..]`
- `literal: []i32 = [1, 2, 3][..]`

## Slices

Slice values are written with `[]T` types and are constructed explicitly from
fixed arrays or other slices:

- `values[..]`
- `values[1..4]`
- `values[..3]`
- `values[2..]`

The result is a view over the original storage. The generated representation is
a struct with:

- `.data`, a pointer to the first element in the slice
- `.count`, the number of elements in the slice

Those fields are available from source when lower-level interop is needed:

- `slice.count`
- `slice.data[0]`

Slices are indexed with the same square-bracket syntax as fixed arrays:

- `slice[0]`
- `slice[i]`

The index expression must be an integer type. In debug builds, generated C emits
a bounds check before each slice index and aborts with a fatal message if the
index is outside `slice.count`. Release builds may omit those checks.

Strings share the same field and slicing surface where it preserves the string
type:

- `text.count`
- `text.data`
- `text[1..4]`
- `text[..]`

String slicing returns `string`, not `[]u8`. Generated C checks that string slice
bounds are within range and on UTF-8 codepoint boundaries before producing the
result. Byte-oriented access remains explicit through `.data`, whose type is
`^u8`.

## Pointers

Pointer types use `^T`, and address-of expressions use the same prefix marker:

- `item_ptr: ^i32 = ^values[0]`
- `array_ptr: ^[3]i32 = ^values`
- `literal_ptr: ^[3]i32 = ^[1, 2, 3]`

Address-of is only valid for addressable values. Current addressable forms are
runtime symbols, fixed-array element indexes, pointer indexes, and array
literals. Taking the address of an array literal produces a pointer to the whole
fixed array, not a slice.

Pointer indexing uses the same `value[index]` syntax as fixed arrays:

- `item_ptr[0]`
- `array_ptr[0][2]`

The index expression must be an integer type. Raw pointer indexing has no known
length in the type system, so generated C does not emit bounds checks for it.

Local variables are resolved through semantic scope rows, not through AST node
payloads. A function body creates a root scope, and each nested block statement
creates a child scope. Locals enter their scope after their initializer has been
resolved, so an initializer cannot reference the variable being declared and
later locals are not visible before their declaration.

Duplicate local names are rejected within the same scope. Inner block scopes may
shadow outer locals, and references resolve to the nearest visible declaration.

Scoped constant declarations are tracked separately from mutable locals. This is
what allows nested function declarations and other scoped `::` bindings to be
predeclared for forward reference within one scope.

## Nested Functions

Nested functions are non-closures. Semantic analysis allows a nested function to
reference:

- globals
- its own parameters
- its own locals

It may not capture parameters or locals from an enclosing function. Capture
attempts produce a dedicated semantic error instead of being lowered through a
hidden environment object.

The lowering strategy keeps this simple:

- semantic analysis records a lowered symbol handle for each function node
- nested functions are mangled by lexical ownership, for example `main$add`
- generated C uses the corresponding Nerd-visible symbol form, for example
  `$main$add`

This keeps the IR and generated C closure-free while still allowing nested
functions and function values.

## Casts

Explicit casts use postfix syntax:

- `value.cast(u8)`
- `128.cast(u8)`

Cast validity is semantic and table-driven. The parser only records a cast node
with a source expression and a target type expression.

The current milestone allows explicit casts across compatible primitive
numeric/bool types and rejects unsupported casts such as `string.cast(u8)` with
structured semantic diagnostics.

## Primitive Operators

Primitive operators are checked semantically and require exact type agreement
within their operator family:

- `+`, `-`, `*`, `/` require matching numeric operands
- `%`, `&`, `^`, `|` require matching integer operands
- `==` and `!=` require matching numeric or `bool` operands
- `<`, `<=`, `>`, `>=` require matching numeric operands
- `!`, `&&`, `||` require `bool` operands

No implicit casts are inserted between operands. Untyped numeric literals may
still be adopted by a surrounding concrete numeric type before that exact-match
check is applied.

## Interpolated Strings

Interpolated strings are typed as `string`, but each `{expr}` segment is
checked independently in sema.

Today sema accepts interpolation of:

- `string`
- `bool`
- concrete integer types
- `f32`
- `f64`
- `untyped integer`, which materialises to `i32` for runtime conversion
- tuples whose elements are all interpolatable
- fixed arrays whose element type is interpolatable
- slices whose element type is interpolatable

Unsupported segment types, such as function values, produce a dedicated
semantic error.

Tuple interpolation renders the value in tuple literal shape, such as
`(1, "one")` or `(1,)` for a one-element tuple. Nested tuples use the same rule
recursively.

Fixed array interpolation renders the value in array literal shape, such as
`[1, 2, 3]`. Nested arrays and tuples use their own literal-shaped rendering
recursively.

Slice interpolation renders the visible slice contents in the same bracketed
shape, such as `[1, 2, 3]`.

The current runtime model also treats interpolated strings as temporary
statement-local values. Sema rejects uses that would let them escape, such as
returning or storing them.

## Type Names

`sema_type_name(...)` renders source-facing type text. It is used by:

- semantic error messages
- LSP hover text
- other tooling that needs human-readable type names

That function should stay aligned with the actual semantic representation.

## Current Scope

Today the semantic type system is focused on:

- built-in primitive types
- function signatures
- alias resolution
- variable storage eligibility
- tuple literals, tuple types, and tuple field access
- fixed array literals, fixed array types, and fixed array indexing
- slice types, explicit slicing, slice fields, and slice indexing
- pointer types, address-of, and pointer indexing
- untyped integer materialisation
- exact-match type checks for the implemented arithmetic surface
- explicit cast validation

One implementation caveat remains: the IR is not yet fully self-describing and
some backend stages still consult semantic tables. A future cleanup should move
that remaining type knowledge into IR so VM-style execution becomes practical.
