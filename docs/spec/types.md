# Types

Source anchors: `src/compiler/ast/parse.c` parses type syntax,
`src/compiler/sema/sema.h` defines semantic type categories,
`src/compiler/sema/sema.c` materialises and checks types, and
`src/compiler/llvm/llvm.c` lowers runtime representation.

## Built-In Types

The semantic analyser recognises these built-in type families:

| Type                               | Role                                                              |
| ---------------------------------- | ----------------------------------------------------------------- |
| `void`                             | No value. Used for statements and functions with no return value. |
| `bool`                             | Boolean value, written with `yes` and `no`.                       |
| `string`                           | Runtime string slice with string-specific typing.                 |
| `i8`, `i16`, `i32`, `i64`, `isize` | Signed integer types.                                             |
| `u8`, `u16`, `u32`, `u64`, `usize` | Unsigned integer types.                                           |
| `f32`, `f64`                       | Floating-point types.                                             |
| `arena`                            | Opaque handle to a pointer-stable arena allocation region.        |

`isize` and `usize` are pointer-sized integer types. Use `usize` for counts,
indexes, byte sizes, and capacities. Use `isize` for signed offsets or other
platform-sized signed integer values.

`untyped integer` and `untyped float` are temporary semantic types for numeric
literals. They materialise to concrete types from context, or to `i32` and
`f64` respectively when no stronger destination type is available.

`nil` is a temporary semantic type that materialises as a nil pointer, nil
slice, or nil dynamic array when the destination type is known.

```nerd
ptr: ^i32 = nil
bytes: []u8 = nil
items: [..]i32 = nil
```

## Type Syntax

```bnf
type            ::= type-name
                  | '(' type ')'
                  | '(' type ',' type-list? ')'
                  | '[]' type
                  | '[' expression ']' type
                  | '[..]' type
                  | '[' expression '..]' type
                  | '^' type
                  | plex-type
                  | union-type
                  | enum-type
                  | function-type

type-name       ::= IDENT { '.' IDENT } { '[' type-list? ']' }
type-list       ::= type { ',' type }

function-type   ::= 'fn' generic-params? '(' type-list? ')' '->' type
```

Function types use unnamed parameter types. Trait requirements reuse this
syntax, for example `show :: fn (Self) -> string`.

Trait implementations attach those required functions to a concrete type:
`impl Display for Point { show :: fn (self: Self) -> string { ... } }`.
The implementation member is callable through the normal receiver method
syntax, for example `point.show()`.
For local trait declarations, implementations must provide every required
member with a compatible function signature after substituting `Self` with the
implementation target type. Duplicate non-generic implementations for the same
trait and target type are rejected. A trait implementation is atomic: all
required members for one trait/type pair must appear in the same
`impl Trait for Type` block. The language does not merge partial
implementations of the same trait for the same type across multiple impl
blocks.

Generic application uses square brackets on a type name, for example
`Map[string, i32]`. Parser support permits an empty bracket list, but the useful
language form is a non-empty type list.

## Aggregate Forms

| Form                   | Description                                                                      |
| ---------------------- | -------------------------------------------------------------------------------- |
| `(A, B)`               | Tuple type. Tuple fields are accessed with numeric field selectors such as `.0`. |
| `[N]T`                 | Fixed array of `N` elements.                                                     |
| `[]T`                  | Slice: pointer plus count.                                                       |
| `[..]T`                | Dynamic array: owned growable storage.                                           |
| `^T`                   | Pointer to `T`.                                                                  |
| `plex { ... }`         | Record-like aggregate with named fields.                                         |
| `plex #c { ... }`      | Plex with C-compatible field layout.                                             |
| `plex #packed { ... }` | Packed plex with C-compatible layout and packed storage.                         |
| `union { ... }`        | Raw union with named alternatives sharing storage.                               |
| `enum { ... }`         | Tagged enum, optionally with payload variants.                                   |

Arrays, slices, strings, and dynamic arrays expose `.data` and `.count` where
semantically valid. Dynamic arrays also expose `.capacity`.

## Aggregate Definitions

```bnf
plex-type       ::= 'plex' generic-params? plex-annotation* '{' plex-field* '}'
union-type      ::= 'union' generic-params? '{' plex-field* '}'
plex-annotation ::= '#c' | '#packed'
plex-field      ::= IDENT type

enum-type       ::= 'enum' generic-params? '{' enum-variant-list? '}'
enum-variant    ::= IDENT [ '(' type-list? ')' ] [ '=' expression ]
```

Plex and union fields are written as `field Type`, not `field: Type`.
Plex annotations are written after `plex` and before the field body. The parser
recognises `#c` and `#packed`; `#packed` also implies the C-layout flag. These
annotations are not accepted on `union` or `enum`.

Enums support unit variants, payload variants, and explicit discriminant
expressions.

## Dynamic Array Methods

The semantic analyser recognises methods on dynamic arrays:

| Method                | Arguments                       | Result    |
| --------------------- | ------------------------------- | --------- |
| `push`                | item                            | `void`    |
| `append`              | matching slice or dynamic array | `void`    |
| `reserve_to`          | absolute capacity               | `void`    |
| `reserve_extra`       | additional room beyond count    | `void`    |
| `resize_to`           | absolute count                  | `void`    |
| `resize_undefined_to` | absolute count                  | `void`    |
| `extend`              | additional count                | `void`    |
| `extend_undefined`    | additional count                | `void`    |
| `delete`              | integer index                   | `void`    |
| `swap_delete`         | integer index                   | `void`    |
| `pop`                 | none                            | item type |
| `clear`               | none                            | `void`    |
| `free`                | none                            | `void`    |

These are language-recognised method calls rather than ordinary functions from
the standard library.

## Type Aliases And Visibility

A binding whose value parses as type syntax creates a type alias:

```nerd
Score :: i32
Point :: plex { x i32 y i32 }
```

Public plex and union fields are part of the public API. Semantic diagnostics
reject public field types that name private declarations.
