nothing :: fn () {}
main :: fn () {
    pointer := ^nothing()
}
¬
{
  "message": "Type mismatch: expected `addressable value`, found `void`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 24
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 24,
      "length": 1,
      "message": "This expression has type `void`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use std.io

main :: fn () {
    values: [2]i32 = [1, 2]
    pointer: ^i32 = ^values[0]
    prn($"{pointer["zero"]}")
}
¬
{
  "message": "Type mismatch: expected `integer index`, found `string`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 6,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 20,
      "length": 6,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
pointer: ^i32: ^[1, 2]
main :: fn () => pointer[0]
¬
{
  "message": "Type mismatch: expected `^i32`, found `^[2]i32`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 1,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 16,
      "length": 1,
      "message": "This expression has type `^[2]i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value: i32 = nil
}
¬
{
  "message": "Type mismatch: expected `i32`, found `nil`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 2,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 18,
      "length": 3,
      "message": "This expression has type `nil`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    ptr: ^u8 = 0
}
¬
{
  "message": "Type mismatch: expected `^u8`, found `untyped integer`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 1,
      "message": "This integer cannot be used directly as a pointer"
    }
  ],
  "notes": [],
  "help": [
    "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
  ]
}

¬
main :: fn () {
    ptr: ^u8 = 0.as(usize)
}
¬
{
  "message": "Type mismatch: expected `^u8`, found `usize`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 2,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 17,
      "length": 1,
      "message": "This integer cannot be used directly as a pointer"
    }
  ],
  "notes": [],
  "help": [
    "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
  ]
}

¬
take :: fn (ptr: ^u8) {}

main :: fn () {
    take(0)
}
¬
{
  "message": "Type mismatch: expected `^u8`, found `untyped integer`",
  "source_file": "tests/errors/pointers.e",
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
      "message": "This integer cannot be used directly as a pointer"
    }
  ],
  "notes": [],
  "help": [
    "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
  ]
}

¬
make :: fn () -> ^u8 {
    return 0
}

main :: fn () {}
¬
{
  "message": "Type mismatch: expected `^u8`, found `untyped integer`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 1,
      "message": "This integer cannot be used directly as a pointer"
    }
  ],
  "notes": [],
  "help": [
    "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
  ]
}

¬
main :: fn () {
    value: i32 = 4096
    ptr := value.as(^u8)
}
¬
{
  "message": "Cannot cast `i32` to `^u8`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 17,
      "length": 1,
      "message": "This cast is not supported"
    }
  ],
  "notes": [],
  "help": [
    "Use explicit casts only between compatible primitive types."
  ]
}

¬
main :: fn () {
    opaque: ^void = nil
    _typed: ^i32 = opaque
}
¬
{
  "message": "Type mismatch: expected `^i32`, found `^void`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 20,
      "length": 6,
      "message": "This expression has type `^void`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value: i32 = 1
    _wrong: ^u32 = ^value
}
¬
{
  "message": "Type mismatch: expected `^u32`, found `^i32`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 20,
      "length": 1,
      "message": "This expression has type `^i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
callback :: fn () {}
main :: fn () {
    _opaque: ^void = callback
}
¬
{
  "message": "Type mismatch: expected `^void`, found `fn () -> void`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 22,
      "length": 8,
      "message": "This expression has type `fn () -> void`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    bytes: [2]u8 = [1, 2]
    _opaque: ^void = bytes[..]
}
¬
{
  "message": "Type mismatch: expected `^void`, found `[]u8`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 3,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 27,
      "length": 1,
      "message": "This expression has type `[]u8`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    pointer: ^i32 = nil
    _wrong: ^^void = ^pointer
}
¬
{
  "message": "Type mismatch: expected `^^void`, found `^^i32`",
  "source_file": "tests/errors/pointers.e",
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
      "message": "This expression has type `^^i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { pointer := ^nil }
¬
{
  "message": "Type mismatch: expected `addressable value`, found `nil`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 1,
    "column": 29
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 29,
      "length": 3,
      "message": "This expression has type `nil`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { pointer := ^i32 }
¬
{
  "message": "Unknown symbol `i32`",
  "source_file": "tests/errors/pointers.e",
  "primary_location": {
    "line": 1,
    "column": 29
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 29,
      "length": 3,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `i32` or fix the spelling."
  ]
}
