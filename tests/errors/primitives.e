left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left % right
¬
{
  "message": "Operator `%` requires matching integer operands, found `f64` and `f64`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 1,
      "message": "These operands have types `f64` and `f64`"
    }
  ],
  "notes": [],
  "help": [
    "Use `%` only with matching integer operands."
  ]
}

¬
left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left & right
¬
{
  "message": "Operator `&` requires matching integer operands, found `f64` and `f64`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 1,
      "message": "These operands have types `f64` and `f64`"
    }
  ],
  "notes": [],
  "help": [
    "Use `&` only with matching integer operands."
  ]
}

¬
left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left << right
¬
{
  "message": "Operator `<<` requires matching integer operands, found `f64` and `f64`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 2,
      "message": "These operands have types `f64` and `f64`"
    }
  ],
  "notes": [],
  "help": [
    "Use `<<` only with matching integer operands."
  ]
}

¬
left: f64 = 1.5
right: f64 = 1.0
main :: fn () => left >> right
¬
{
  "message": "Operator `>>` requires matching integer operands, found `f64` and `f64`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 2,
      "message": "These operands have types `f64` and `f64`"
    }
  ],
  "notes": [],
  "help": [
    "Use `>>` only with matching integer operands."
  ]
}

¬
left: i32 = 1
right: i32 = 2
main :: fn () => left && right
¬
{
  "message": "Operator `&&` requires bool, box, optional or result condition operands, found `i32` and `i32`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 2,
      "message": "These operands have types `i32` and `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Use `&&` only with bool, box, optional or result condition operands."
  ]
}

¬
left: string = "a"
right: string = "b"
main :: fn () => left < right
¬
{
  "message": "Operator `<` requires matching numeric operands, found `string` and `string`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 1,
      "message": "These operands have types `string` and `string`"
    }
  ],
  "notes": [],
  "help": [
    "Use `<` only with matching numeric operands."
  ]
}

¬
'abc
¬
{
  "message": "Unterminated packed integer literal",
  "source_file": "tests/errors/primitives.e",
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
      "message": "Packed integer literal starts here"
    }
  ],
  "notes": [],
  "help": [
    "Add a closing single quote to terminate the literal."
  ]
}

¬
'abcdefghi'
¬
{
  "message": "Packed integer literal is too large",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 10,
      "message": "This literal exceeds the 8-byte limit"
    }
  ],
  "notes": [],
  "help": [
    "Use at most 8 bytes inside one single-quote literal."
  ]
}

¬
0xG
¬
{
  "message": "Invalid character 'G' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character 'G' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
0b2
¬
{
  "message": "Invalid character '2' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character '2' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
0o8
¬
{
  "message": "Invalid character '8' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character '8' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
0x
¬
{
  "message": "Invalid character 'x' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 2,
      "message": "Invalid character 'x' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
io :: use std.io
value :: io.size
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `runtime-sized value`, found `module`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 13,
      "length": 4,
      "message": "This expression has type `module`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
1_
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 2,
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
1__000
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
1.0_
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
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
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
1.0__0
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
1._0
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
main :: fn () {
    while !yes {
    }
}
¬
{
  "message": "Unknown symbol `while`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `while` or fix the spelling."
  ]
}

¬
impl [T] []T {
    contains :: fn (s: ^Self, value: T) -> bool {
        return s^[0] == value
    }
}
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 22,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
same :: fn [T] (lhs: T, rhs: T) => lhs == rhs
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 40,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
same :: fn [T] (lhs: []T, rhs: []T) -> bool { return lhs == rhs }
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 58
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 58,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
same :: fn [T] (lhs: box[T], rhs: box[T]) -> bool { return lhs == rhs }
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 64
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 64,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
same :: fn [T] (lhs: ^T, rhs: T) -> bool { return lhs^ == rhs }
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 56
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 56,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
impl [T] []T {
    before :: fn (s: ^Self, value: T) -> bool { return s^[0] < value }
}
main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Order` constraint",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 62
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 62,
      "length": 1,
      "message": "This operation requires `T: Order`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Order` to the generic function or impl."
  ]
}

¬
main :: fn () {
    lhs: arena
    rhs: arena
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `arena` and `arena`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 4,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 16,
      "length": 2,
      "message": "These operands have types `arena` and `arena`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: Bits
    rhs: Bits
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `Bits` and `Bits`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `Bits` and `Bits`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Bits` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
one :: fn () => 1
main :: fn () { assert one == one }
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `fn () -> i32` and `fn () -> i32`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 28
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 28,
      "length": 2,
      "message": "These operands have types `fn () -> i32` and `fn () -> i32`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Value :: plex { x arena }
main :: fn () {
    lhs: []Value
    rhs: []Value
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `[]Value` and `[]Value`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `[]Value` and `[]Value`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Value` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
main :: fn () {
    lhs: []arena
    rhs: []arena
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `[]arena` and `[]arena`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 4,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 16,
      "length": 2,
      "message": "These operands have types `[]arena` and `[]arena`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: []Bits
    rhs: []Bits
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `[]Bits` and `[]Bits`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `[]Bits` and `[]Bits`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Bits` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Callback :: fn () -> i32
main :: fn () {
    lhs: []Callback
    rhs: []Callback
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `[]fn () -> i32` and `[]fn () -> i32`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `[]fn () -> i32` and `[]fn () -> i32`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
main :: fn () {
    lhs: box[arena]
    rhs: box[arena]
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `box[arena]` and `box[arena]`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 4,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 16,
      "length": 2,
      "message": "These operands have types `box[arena]` and `box[arena]`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Bits :: union { integer i32 real f32 }
main :: fn () {
    lhs: box[Bits]
    rhs: box[Bits]
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `box[Bits]` and `box[Bits]`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `box[Bits]` and `box[Bits]`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Bits` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Callback :: fn () -> i32
main :: fn () {
    lhs: box[Callback]
    rhs: box[Callback]
    assert lhs == rhs
}
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `box[fn () -> i32]` and `box[fn () -> i32]`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 5,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 16,
      "length": 2,
      "message": "These operands have types `box[fn () -> i32]` and `box[fn () -> i32]`"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
main :: fn () {
    lhs: []arena
    rhs: []arena
    assert same(lhs, rhs)
}
¬
{
  "message": "Type mismatch: expected `Eq implementation`, found `[]arena`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 6,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 17,
      "length": 3,
      "message": "This expression has type `[]arena`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/mods/core.n",
      "line": 204,
      "column": 5,
      "length": 2,
      "message": "Type `Eq` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
main :: fn () {
    lhs: box[arena]
    rhs: box[arena]
    assert same(lhs, rhs)
}
¬
{
  "message": "Type mismatch: expected `Eq implementation`, found `box[arena]`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 6,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 17,
      "length": 3,
      "message": "This expression has type `box[arena]`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/mods/core.n",
      "line": 204,
      "column": 5,
      "length": 2,
      "message": "Type `Eq` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Bad :: plex { resource arena }
main :: fn () { a: Bad b: Bad assert a == b }
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `Bad` and `Bad`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 40,
      "length": 2,
      "message": "These operands have types `Bad` and `Bad`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Type `Bad` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Bad :: enum { Empty Resource(arena) }
main :: fn () { a: Bad = Empty b: Bad = Empty assert a == b }
¬
{
  "message": "Operator `==` requires matching operands that support Eq, found `Bad` and `Bad`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 56
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 56,
      "length": 2,
      "message": "These operands have types `Bad` and `Bad`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Type `Bad` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `==` only with matching operands that support Eq."
  ]
}

¬
Bad :: enum { Empty Callback(fn () -> i32) }
same :: fn [T] (a:T,b:T) -> bool where T: Eq { return a == b }
main :: fn () { a: Bad = Empty assert same(a,a) }
¬
{
  "message": "Type mismatch: expected `Eq implementation`, found `Bad`",
  "source_file": "tests/errors/primitives.e",
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
      "message": "This expression has type `Bad`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/mods/core.n",
      "line": 204,
      "column": 5,
      "length": 2,
      "message": "Type `Eq` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/primitives.e",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Type `Bad` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u8 = 256
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: atomic[u8] = 256
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 22,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i8 = 128
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i8 = -129
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u8 = -1
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u16 = 65536
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u16`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 5,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i16 = -32769
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i16`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u32 = 4294967296
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i32 = 2147483648
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i64 = 9223372036854775808
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i64`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 19,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: i64 = -9223372036854775809
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i64`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u64 = -1
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u64`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
N :: 256
main :: fn () {
 value: u8 = N
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 14,
      "length": 1,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 original := 256
 value: u8 = original
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
take :: fn (value: u8) { _ := value }
main :: fn () { take(256) }
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 22,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () -> u8 { return 256 }
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 30
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 30,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
P :: plex { count u8 }
main :: fn () {
 value: P = {count: 256}
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 21,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: [2]u8 = [0, 256]
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 21,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value: u8 = 0
 value = 256
 _ := value
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 3,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 10,
      "length": 3,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value := 2147483648
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 11,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
value := 2147483648
main :: fn () { _ := value }
¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 10,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value := [2147483648]
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
 value := (2147483648, 1)
 _ := value
}
¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 10,
      "message": "This expression has type `out-of-range integer literal`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
value : i32 : 2.0
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `i32`, found `untyped float`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 15,
      "length": 3,
      "message": "This expression has type `untyped float`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
number :: 2.0
value : i32 : number
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `i32`, found `untyped float`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 6,
      "message": "This expression has type `untyped float`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
get_value :: fn () -> i32 { return 2.0 }
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `i32`, found `untyped float`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 36
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 36,
      "length": 3,
      "message": "This expression has type `untyped float`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
number : i32 : 2
value : f32 : number
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `f32`, found `i32`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 15,
      "length": 6,
      "message": "This expression has type `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
value :: true
main :: fn () {}
¬
{
  "message": "Unknown symbol `true`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 10,
      "length": 4,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `true` or fix the spelling."
  ]
}

¬
value :: false
main :: fn () {}
¬
{
  "message": "Unknown symbol `false`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 10,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `false` or fix the spelling."
  ]
}

¬
main :: fn () -> bool { return true }
¬
{
  "message": "Unknown symbol `true`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 32
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 32,
      "length": 4,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `true` or fix the spelling."
  ]
}

¬
main :: fn () { on false => prn("unexpected") }
¬
{
  "message": "Unknown symbol `false`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 20,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `false` or fix the spelling."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 2,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e+
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 3,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e-
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 3,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e--3
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 3,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1E+-3
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 3,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e_3
}
¬
{
  "message": "Scientific exponent requires decimal digits",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 2,
      "message": "Expected digits after exponent marker and optional sign"
    }
  ],
  "notes": [],
  "help": [
    "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e3e2
}
¬
{
  "message": "Invalid character 'e' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 19,
      "length": 4,
      "message": "Invalid character 'e' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
main :: fn () {
    _value: f64 = 1e3_
}
¬
{
  "message": "Invalid character '_' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 2,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 19,
      "length": 4,
      "message": "Invalid character '_' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}


¬
21n
¬
{
  "message": "Invalid character 'n' in number literal",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 3,
      "message": "Invalid character 'n' in number literal"
    }
  ],
  "notes": [],
  "help": [
    "Review the number literal for typos or insert whitespace."
  ]
}

¬
main :: fn () => "hi".as(u8)
¬
{
  "message": "Cannot cast `string` to `u8`",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 22,
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
18446744073709551616
¬
{
  "message": "Integer literal is too large",
  "source_file": "tests/errors/primitives.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 20,
      "message": "Literal overflow starts here"
    }
  ],
  "notes": [],
  "help": [
    "Use a smaller integer literal."
  ]
}
