use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    prn($"{p.z}")
}
¬
{
  "message": "Unknown member `z` for `Point`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 10,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 14,
      "length": 1,
      "message": "`Point` has no field or method named `z`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 3,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Did you mean `.x`?"
  ]
}

¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1 }
    prn($"{p.x}")
}
¬
{
  "message": "Plex literal is missing required field",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 9,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 16,
      "length": 1,
      "message": "This literal does not initialise every required field"
    }
  ],
  "notes": [
    "Missing field: `y`"
  ],
  "help": [
    "Add all fields required by the plex type, or write `...` in the literal to initialise omitted fields with their default values."
  ]
}

¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, x: 2 }
    prn($"{p.x}")
}
¬
{
  "message": "Type mismatch: expected `unique plex field`, found `x`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 9,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 27,
      "length": 1,
      "message": "This expression has type `x`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: "one", y: 2 }
    prn($"{p.x}")
}
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 9,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 21,
      "length": 5,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { z: 3 }
    prn($"{q.x}")
}
¬
{
  "message": "Unknown field `z` in plex literal of type `Point`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 10,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 19,
      "length": 1,
      "message": "The target plex type has no field named `z`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 3,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Did you mean `x`?"
  ]
}

¬
Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { x: 3, x: 4 }
    prn($"{q.x}")
}
¬
{
  "message": "Type mismatch: expected `unique plex field`, found `x`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 8,
    "column": 28
  },
  "references": [
    {
      "kind": "primary",
      "line": 8,
      "column": 28,
      "length": 1,
      "message": "This expression has type `x`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { x: "wrong" }
    prn($"{q.x}")
}
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 8,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 8,
      "column": 22,
      "length": 7,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    x := 1
    y := x with { value: 2 }
    prn($"{y}")
}
¬
{
  "message": "Type mismatch: expected `plex value`, found `i32`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 12,
      "length": 4,
      "message": "This expression has type `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { i: 1, f: 2.0 }
}
¬
{
  "message": "Type mismatch: expected `one union field`, found `different field count`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 22,
      "length": 1,
      "message": "This expression has type `different field count`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Value :: union { i i32 f f32 }
main :: fn () -> i32 {
    v: Value = Value { i: 1 }
    return on v {
        else => 0
    }
}
¬
{
  "message": "Block-form `on` does not support values of type `Value`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 15,
      "length": 1,
      "message": "This matched value type is unsupported"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Value` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Block-form `on` supports `bool` and `string` scrutinees, plus concrete integer scrutinees."
  ]
}

¬
Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value {}
}
¬
{
  "message": "Type mismatch: expected `one union field`, found `different field count`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 22,
      "length": 1,
      "message": "This expression has type `different field count`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { text: "bad" }
}
¬
{
  "message": "Unknown field `text` in union literal of type `Value`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 24
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 24,
      "length": 4,
      "message": "The target union type has no field named `text`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Value` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use a field declared by the target union type."
  ]
}

¬
Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { i: 1 }
    w := v with { f: 2.0 }
}
¬
{
  "message": "Type mismatch: expected `plex value`, found `Value`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 12,
      "length": 4,
      "message": "This expression has type `Value`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Value` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Location :: plex {
    description string
    tag         string
}

Object :: plex {
    description string
    tag         string
    location    ^Location
}

objs: []Object = [
    { description: "x", tag: "y", location: ^objs[0] },
]
¬
{
  "message": "Type mismatch: expected `^Location`, found `^Object`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 13,
    "column": 45
  },
  "references": [
    {
      "kind": "primary",
      "line": 13,
      "column": 45,
      "length": 1,
      "message": "This expression has type `^Object`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 8,
      "message": "Type `Location` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 6,
      "column": 1,
      "length": 6,
      "message": "Type `Object` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Node :: plex {
    next Node
}
¬
{
  "message": "Type alias cycle involving `Node`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "This alias participates in a type cycle"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Cycle closes through `Node` here"
    }
  ],
  "notes": [],
  "help": [
    "Break the cycle by rewriting one of the aliases so it resolves to a concrete type."
  ]
}

¬
Point :: plex { x i32 y i32 }

main :: fn () {
    p := { x: 1, y: 2 }
}
¬
{
  "message": "Type mismatch: expected `plex or union type`, found `<unknown>`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 10,
      "length": 1,
      "message": "This expression has type `<unknown>`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Bad :: plex {
    field ^Missing
}
¬
{
  "message": "Unknown type `Missing`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 7,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
Alias :: Missing

Bad :: plex {
    cb fn (value: Alias) -> i32
}
¬
{
  "message": "Unknown type `Missing`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 1,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 10,
      "length": 7,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
Bad :: plex {
    field: i32
}
main :: fn () {}
¬
{
  "message": "Expected type but found Colon `:`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 2,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 10,
      "length": 1,
      "message": "A type is expected here"
    }
  ],
  "notes": [
    "Plex field definitions are written as `field Type`."
  ],
  "help": [
    "Remove the colon. Colons are used in plex literals such as `State { loc_index: 0 }`, not in plex definitions."
  ]
}

¬
Bad :: plex {
    cb fn (value: Missing) -> i32
}
¬
{
  "message": "Unknown type `Missing`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 2,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 19,
      "length": 7,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
Private :: plex {
    value i32
}

pub Public :: plex {
    field Private
}

main :: fn () {}
¬
{
  "message": "Public type `Public` exposes private type `Private`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 6,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 11,
      "length": 7,
      "message": "`Private` is private to this module"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Private` is defined here"
    }
  ],
  "notes": [
    "Public plex and union fields are part of the module's public API."
  ],
  "help": [
    "Make `Private` public or hide it behind a private implementation detail that is not stored in the public field list."
  ]
}

¬
Private :: plex {
    value i32
}

pub Public :: plex {
    field ^Private
}

main :: fn () {}
¬
{
  "message": "Public type `Public` exposes private type `Private`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 6,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 12,
      "length": 7,
      "message": "`Private` is private to this module"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Private` is defined here"
    }
  ],
  "notes": [
    "Public plex and union fields are part of the module's public API."
  ],
  "help": [
    "Make `Private` public or hide it behind a private implementation detail that is not stored in the public field list."
  ]
}

¬
Header :: plex {
    i8 {
        version : 4
    }
}
¬
{
  "message": "Type mismatch: expected `unsigned integer bit-field storage type`, found `i8`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 2,
      "message": "This expression has type `i8`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
TokenType :: enum { Eof Data(u8) }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}
¬
{
  "message": "Type mismatch: expected `payload-free enum whose discriminants fit the bit width`, found `TokenType`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 9,
      "length": 4,
      "message": "This expression has type `TokenType`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 9,
      "message": "Type `TokenType` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
TokenType :: enum { Eof Last = 16 }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}
¬
{
  "message": "Type mismatch: expected `payload-free enum whose discriminants fit the bit width`, found `TokenType`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 9,
      "length": 4,
      "message": "This expression has type `TokenType`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 9,
      "message": "Type `TokenType` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
TokenType :: enum { Eof }

Header :: plex {
    u8 {
        _ TokenType : 4
        kind        : 4
    }
}
¬
{
  "message": "Type mismatch: expected `untyped bit-field padding`, found `typed padding`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 9,
      "length": 1,
      "message": "This expression has type `typed padding`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        version : 0
    }
}
¬
{
  "message": "Type mismatch: expected `positive compile-time bit width within storage`, found `invalid or overflowing bit width`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 9,
      "length": 7,
      "message": "This expression has type `invalid or overflowing bit width`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        version : 5
        kind    : 4
    }
}
¬
{
  "message": "Type mismatch: expected `positive compile-time bit width within storage`, found `invalid or overflowing bit width`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 4,
      "message": "This expression has type `invalid or overflowing bit width`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        _ : 8
    }
}
¬
{
  "message": "Type mismatch: expected `bit-field block with a named field`, found `padding-only bit-field block`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 2,
      "message": "This expression has type `padding-only bit-field block`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    _ := Header { version: 16 kind: 0 }
}
¬
{
  "message": "Type mismatch: expected `value fitting 4-bit field`, found `out-of-range integer constant`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 9,
    "column": 28
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 28,
      "length": 2,
      "message": "This expression has type `out-of-range integer constant`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
TokenType :: enum { Eof Data }

Header :: plex {
    u8 {
        kind TokenType : 4
    }
}

main :: fn () {
    header := Header { kind: TokenType.Eof }
    header.kind = 1
}
¬
{
  "message": "Type mismatch: expected `TokenType`, found `untyped integer`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 11,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 11,
      "column": 19,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 9,
      "message": "Type `TokenType` is defined here"
    }
  ],
  "notes": [
    "`kind` is a bitfield of type `TokenType`."
  ],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    header := Header { version: 0 kind: 0 }
    header.version = 16
}
¬
{
  "message": "Type mismatch: expected `value fitting 4-bit field`, found `out-of-range integer constant`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 10,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 12,
      "length": 7,
      "message": "This expression has type `out-of-range integer constant`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
}

main :: fn () {
    header := Header { version: 0 kind: 0 }
    _ := ^header.version
}
¬
{
  "message": "Type mismatch: expected `addressable value`, found `u8`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 10,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 18,
      "length": 7,
      "message": "This expression has type `u8`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value := {
        first: 1
        second = 2
    }
    _ := value
}
¬
{
  "message": "Expected Symbol but found Equal `=`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 16,
      "length": 1,
      "message": "Found Equal `=` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
Token :: plex {
    u32 {
        offset : 24
        _      : 8
    }
}

main :: fn () {
    start := 0.as(usize)
    _ := Token { offset: start }
}
¬
{
  "message": "Type mismatch: expected `u32`, found `usize`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 10,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 26,
      "length": 5,
      "message": "This expression has type `usize`"
    }
  ],
  "notes": [
    "`offset` is a bitfield of type `u32`."
  ],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Token :: plex {
    u32 {
        offset : 24
        _      : 8
    }
}

main :: fn () {
    start := 0.as(usize)
    token := Token { offset: 0 }
    token.offset = start
}
¬
{
  "message": "Type mismatch: expected `u32`, found `usize`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 11,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 11,
      "column": 20,
      "length": 5,
      "message": "This expression has type `usize`"
    }
  ],
  "notes": [
    "`offset` is a bitfield of type `u32`."
  ],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use inferred_fill
use structural_eq
main :: fn () {
    count := 2
    items: [count]Item
    items.fill_context(^{ missing: 1, ... })
}
¬
{
  "message": "Unknown field `missing` in plex literal of type `Item`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 6,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 27,
      "length": 7,
      "message": "The target plex type has no field named `missing`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/tests/mods/structural_eq.n",
      "line": 1,
      "column": 5,
      "length": 4,
      "message": "Type `Item` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use a field declared by the target plex type."
  ]
}

¬
use inferred_fill
Record :: plex { value i32 }
main :: fn () {
    items: [2]Record
    fill(items, ^{ missing: 1, ... })
}
¬
{
  "message": "Unknown field `missing` in plex literal of type `Record`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 20,
      "length": 7,
      "message": "The target plex type has no field named `missing`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 6,
      "message": "Type `Record` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use a field declared by the target plex type."
  ]
}


¬
Point :: plex { x i32 }
main :: fn () { point := Point { x: 1 }
 { x: a, x: b } := point
 _ := a + b }
¬
{
  "message": "Type mismatch: expected `unique destructuring field`, found `x`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 10,
      "length": 1,
      "message": "This expression has type `x`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex {
    x i32
    y i32
}

Rect :: plex {
    x u32
    use Point
}

main :: fn () {}
¬
{
  "message": "Embedded plex field `x` conflicts with another field",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 8,
    "column": 5
  },
  "references": [],
  "notes": [],
  "help": [
    "Rename the field or remove one of the conflicting plex uses."
  ]
}


¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { p: Point _r: Rect = p }
¬
{
  "message": "Type mismatch: expected `Rect`, found `Point`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 37
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 37,
      "length": 1,
      "message": "This expression has type `Point`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { p: ^Point _r: ^Rect = p }
¬
{
  "message": "Type mismatch: expected `^Rect`, found `^Point`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 39
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 39,
      "length": 1,
      "message": "This expression has type `^Point`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
Plain :: plex { x f32 y f32 w f32 h f32 }
main :: fn () { r: Plain _p: Point = r }
¬
{
  "message": "Type mismatch: expected `Point`, found `Plain`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 38,
      "length": 1,
      "message": "This expression has type `Plain`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 3,
      "column": 1,
      "length": 5,
      "message": "Type `Plain` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: Rect _p: ^Point = r }
¬
{
  "message": "Type mismatch: expected `^Point`, found `Rect`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 38,
      "length": 1,
      "message": "This expression has type `Rect`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: Rect _p := r.as(^Point) }
¬
{
  "message": "Cannot cast `Rect` to `^Point`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 32
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 32,
      "length": 1,
      "message": "This cast is not supported"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use explicit casts only between compatible primitive types."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: []^Rect _p: []^Point = r }
¬
{
  "message": "Type mismatch: expected `[]^Point`, found `[]^Rect`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 43
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 43,
      "length": 1,
      "message": "This expression has type `[]^Rect`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
main :: fn () { r: ^^Rect _p: ^^Point = r }
¬
{
  "message": "Type mismatch: expected `^^Point`, found `^^Rect`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 41
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 41,
      "length": 1,
      "message": "This expression has type `^^Rect`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Pair :: plex [T] { first T }
IntPair :: plex { use Pair[i32] extra u8 }
main :: fn () { r: IntPair _p: Pair[f32] = r }
¬
{
  "message": "Type mismatch: expected `plex { f32 first }`, found `IntPair`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 3,
    "column": 44
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 44,
      "length": 1,
      "message": "This expression has type `IntPair`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 7,
      "message": "Type `IntPair` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
Other :: plex { colour u8 }
Both :: plex { use Point use Other }
a :: fn (_p: ^Point) -> i32 { return 1 }
b :: fn (_p: ^Other) -> i32 { return 2 }
choose :: fn { a b }
main :: fn () { r: Both _v := choose(^r) }
¬
{
  "message": "Type mismatch: expected `exactly one compatible compound function member`, found `ambiguous compound call`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 8,
    "column": 37
  },
  "references": [
    {
      "kind": "primary",
      "line": 8,
      "column": 37,
      "length": 1,
      "message": "This expression has type `ambiguous compound call`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Empty :: plex {}
Left :: plex { left u8 use Empty }
Right :: plex { right u8 use Empty }
Both :: plex { use Left use Right }
main :: fn () { r: Both _p: ^Empty = ^r }
¬
{
  "message": "Type mismatch: expected `unique plex use path`, found `ambiguous plex use paths`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 38,
      "length": 1,
      "message": "This expression has type `ambiguous plex use paths`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Empty :: plex {}
Left :: plex { left u8 use Empty }
Right :: plex { right u8 use Empty }
Both :: plex { use Left use Right }
main :: fn () { r: Both _p := (^r).as(^Empty) }
¬
{
  "message": "Type mismatch: expected `unique plex use path`, found `ambiguous plex use paths`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 5,
    "column": 35
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 35,
      "length": 1,
      "message": "This expression has type `ambiguous plex use paths`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x f32 y f32 }
Rect :: plex { use Point w f32 h f32 }
rect_only :: fn (_r: ^Rect) {}
main :: fn () { _f: fn (p: ^Point) = rect_only }
¬
{
  "message": "Type mismatch: expected `fn (^Point) -> void`, found `fn (^Rect) -> void`",
  "source_file": "tests/errors/records.e",
  "primary_location": {
    "line": 4,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 38,
      "length": 9,
      "message": "This expression has type `fn (^Rect) -> void`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/records.e",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "Type `Rect` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}
