main :: fn () {
    value := 1
    value := 2
    return value
}
¬
{
  "message": "Duplicate binding for symbol `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 5,
      "length": 5,
      "message": "This binding redefines `value`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "Previous binding of `value` is here"
    }
  ],
  "notes": [],
  "help": [
    "Rename one of the bindings or remove the duplicate definition."
  ]
}

¬
main :: fn () {
    value := later
    later := 1
    return value
}
¬
{
  "message": "Unknown symbol `later`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `later` or fix the spelling."
  ]
}

¬
main :: fn () {
    value := value + 1
    return value
}
¬
{
  "message": "Unknown symbol `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 14,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `value` or fix the spelling."
  ]
}

¬
main :: fn () {
    {
        inner := 1
    }
    return inner
}
¬
{
  "message": "Unknown symbol `inner`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 5,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 12,
      "length": 5,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `inner` or fix the spelling."
  ]
}

¬
use std.io

value: i32: ${
    prn("fallthrough")
}
main :: fn () => value
¬
{
  "message": "Missing `break` for expression block returning `i32`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 13,
      "length": 1,
      "message": "This expression block can reach the end without breaking with a `i32` value"
    }
  ],
  "notes": [
    "Value-producing expression blocks must exit every reachable path with `break <expr>`."
  ],
  "help": [
    "Add `break <expr>` before the block ends or use a void context if the block should not produce a value."
  ]
}

¬
value: i32: ${
    break "bad"
}
main :: fn () => value
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 11,
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
main :: fn () {
    value :: ${
        again
    }
    return 0
}
¬
{
  "message": "`again` can only be used inside a loop",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 9,
      "length": 5,
      "message": "This `again` is not inside a `for` loop"
    }
  ],
  "notes": [],
  "help": [
    "Move `again` into a `for` loop body."
  ]
}

¬
main :: fn () {
    break
}
¬
{
  "message": "`break` can only be used inside a loop or expression block",
  "source_file": "tests/errors/bindings.e",
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
      "message": "This `break` is not inside a `for` loop or expression block"
    }
  ],
  "notes": [],
  "help": [
    "Move `break` into a `for` loop or expression block."
  ]
}

¬
main :: fn () {
    value :: $answer {
        break $missing 7
    }
    return value
}
¬
{
  "message": "Unknown control label `$missing`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 9,
      "length": 5,
      "message": "No enclosing expression block or loop has this label"
    }
  ],
  "notes": [],
  "help": [
    "Use the label from an enclosing `$label { ... }` block or `for ... $label { ... }` loop."
  ]
}

¬
main :: fn () -> i32 {
    value: i32 = undefined
    return value
}
¬
{
  "message": "Cannot read `value` before it has been assigned",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 12,
      "length": 5,
      "message": "`value` is read here"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "`value` is declared with `undefined` here"
    }
  ],
  "notes": [
    "Variables declared with `undefined` must be assigned before they are read."
  ],
  "help": [
    "Assign to `value` on every path before using its value."
  ]
}

¬
check :: fn (flag: bool) -> i32 {
    value: i32 = undefined
    on flag => value = 1
    return value
}

main :: fn () -> i32 {
    return check(yes)
}
¬
{
  "message": "Cannot read `value` before it has been assigned",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 4,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 12,
      "length": 5,
      "message": "`value` is read here"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "`value` is declared with `undefined` here"
    }
  ],
  "notes": [
    "Variables declared with `undefined` must be assigned before they are read."
  ],
  "help": [
    "Assign to `value` on every path before using its value."
  ]
}

¬
main :: fn () -> i32 {
    value: i32 = undefined
    for yes {
        value = 1
    }
    return value
}
¬
{
  "message": "Cannot read `value` before it has been assigned",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 6,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 12,
      "length": 5,
      "message": "`value` is read here"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "`value` is declared with `undefined` here"
    }
  ],
  "notes": [
    "Variables declared with `undefined` must be assigned before they are read."
  ],
  "help": [
    "Assign to `value` on every path before using its value."
  ]
}

¬
main :: fn () -> i32 {
    value := 1
    return 0
}
¬
{
  "message": "Unused local variable `value`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "This local variable is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `value` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
main :: fn () -> i32 {
    return helper(1)
}

helper :: fn (value: i32) -> i32 {
    return 0
}
¬
{
  "message": "Unused parameter `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 5,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 15,
      "length": 5,
      "message": "This parameter is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `value` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
Bad :: plex {
    field MissingType
}
main :: fn () {}
¬
{
  "message": "Unknown type `MissingType`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 11,
      "length": 11,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
value :: fn () -> i32 {
    return 1
}

main :: fn () {
    value()
}
¬
{
  "message": "Expression result of type `i32` is not used",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 6,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 5,
      "length": 5,
      "message": "This expression produces a value, but the value is discarded"
    }
  ],
  "notes": [
    "Only `void` expressions can be used as standalone statements."
  ],
  "help": [
    "Bind the result to `_` when the value is intentionally ignored."
  ]
}

¬
main :: fn () -> i32 {
    _result := 1
    return _result
}
¬
{
  "message": "Used local variable `_result` marked as unused",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 12,
      "length": 7,
      "message": "This read uses `_result`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 7,
      "message": "`_result` is marked unused by its leading `_`"
    }
  ],
  "notes": [
    "Leading `_` names are reserved for bindings that are deliberately unused."
  ],
  "help": [
    "Rename `_result` without the leading `_` now that it is used."
  ]
}

¬
Thing :: plex {}

impl Thing {
    pub use :: fn (self: Self) {}
}

main :: fn () {}
¬
{
  "message": "Keyword `use` cannot be used as a binding name",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 3,
      "message": "`use` is reserved for the language grammar"
    }
  ],
  "notes": [],
  "help": [
    "Choose a non-keyword name such as `bind`, `activate`, or `use_shader`."
  ]
}

¬
pub BLACK :: i32 = 0xff000000

main :: fn () {}
¬
{
  "message": "Invalid typed variable declaration",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 1,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 14,
      "length": 3,
      "message": "`i32` is a type, not an assignment target"
    }
  ],
  "notes": [
    "`::` starts a constant binding, so the following `=` is parsed as an assignment"
  ],
  "help": [
    "Use `name : i32 = value` for a typed variable declaration"
  ]
}

¬
Failure :: enum {
    Bad
}

get :: fn () -> i32\Failure {
    return 1
}

main :: fn () {
    on get() => [value] {
    }
}
¬
{
  "message": "Unused pattern binder `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 10,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 18,
      "length": 5,
      "message": "This pattern binder is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `value` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
Left :: plex { x i32 }
Right :: plex { x bool }
main :: fn () {
    a: Left
    _b: Right = a
}
¬
{
  "message": "Type mismatch: expected `Right`, found `Left`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 5,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 17,
      "length": 1,
      "message": "This expression has type `Left`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 2,
      "column": 1,
      "length": 5,
      "message": "Type `Right` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Left` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Item :: plex { x i32 }
main :: fn () {
    a: Item
    _sum := a + a
}
¬
{
  "message": "Operator `+` requires matching numeric operands, found `Item` and `Item`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "These operands have types `Item` and `Item`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Item` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `+` only with matching numeric operands."
  ]
}

¬
Left :: enum { One }
Right :: enum { Two }
main :: fn () {
    a: Left = One
    _b: Right = a
}
¬
{
  "message": "Type mismatch: expected `Right`, found `Left`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 5,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 17,
      "length": 1,
      "message": "This expression has type `Left`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 2,
      "column": 1,
      "length": 5,
      "message": "Type `Right` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Left` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Left :: plex { x i32 }
Right :: plex { y bool }
main :: fn () {
    a: Left
    _b: ^Right = ^a
}
¬
{
  "message": "Type mismatch: expected `^Right`, found `^Left`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 5,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 18,
      "length": 1,
      "message": "This expression has type `^Left`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 2,
      "column": 1,
      "length": 5,
      "message": "Type `Right` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Left` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    _b: bool = 3
}
¬
{
  "message": "Type mismatch: expected `bool`, found `untyped integer`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Point :: plex { x i32 }
main :: fn () { p :: Point { x: 20 } p.x = 42 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 1,
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
main :: fn () { p :: Point { x: 20 } p.x += 1 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 1,
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
p :: Point { x: 20 }
main :: fn () { p.x = 42 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
p: Point: Point { x: 20 }
main :: fn () { p.x += 1 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
Holder :: plex { point Point }
main :: fn () { p :: Holder { point: Point { x: 20 } } p.point.x = 42 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 56
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 56,
      "length": 1,
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
main :: fn () { p :: [Point { x: 20 }] p[0].x = 42 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 40,
      "length": 1,
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Point :: plex { x i32 }
main :: fn () { original :: Point { x: 20 } p :: original p.x = 42 }
¬
{
  "message": "Cannot assign to `p`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 59
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 59,
      "length": 1,
      "message": "`p` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `p` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Value :: union { integer i32 real f32 }
main :: fn () { value :: Value { integer: 20 } value.integer = 42 }
¬
{
  "message": "Cannot assign to `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 48
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 48,
      "length": 5,
      "message": "`value` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `value` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Header :: plex { u8 { count: 4 _: 4 } }
main :: fn () { header :: Header { count: 1 } header.count += 1 }
¬
{
  "message": "Cannot assign to `header`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 47
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 47,
      "length": 6,
      "message": "`header` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `header` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
support :: use tests.mods.immutable_aggregate_bindings
main :: fn () { support.point.x = 42 }
¬
{
  "message": "Cannot assign to `point`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 5,
      "message": "`point` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `point` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
use tests.mods.immutable_aggregate_bindings
main :: fn () { point.x = 42 }
¬
{
  "message": "Cannot assign to `point`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 17,
      "length": 5,
      "message": "`point` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `point` as a variable with `:` or assign to a different mutable symbol."
  ]
}


¬
main :: fn () {
    _numbers := [10 ..]i32
}
¬
{
  "message": "Type used where a value was expected",
  "source_file": "tests/errors/bindings.e",
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
      "message": "This begins type syntax"
    }
  ],
  "notes": [
    "Bindings declared with `:=` require a runtime initializer"
  ],
  "help": [
    "Use `_numbers: Type` to declare a default-initialized variable, or provide a runtime value after `:=`."
  ]
}

¬
main :: fn () => mystery
¬
{
  "message": "Unknown symbol `mystery`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 1,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 18,
      "length": 7,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `mystery` or fix the spelling."
  ]
}

¬
answer :: 42
answer :: 7
¬
{
  "message": "Duplicate binding for symbol `answer`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 1,
      "length": 6,
      "message": "This binding redefines `answer`"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "Previous binding of `answer` is here"
    }
  ],
  "notes": [],
  "help": [
    "Rename one of the bindings or remove the duplicate definition."
  ]
}

¬
value: mystery: 1
¬
{
  "message": "Unknown type `mystery`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 1,
    "column": 8
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 8,
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
value: string: 1
¬
{
  "message": "Type mismatch: expected `string`, found `untyped integer`",
  "source_file": "tests/errors/bindings.e",
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
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
value :: 1
main :: fn () {
    value = 2
    return value
}
¬
{
  "message": "Cannot assign to `value`",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 3,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 5,
      "length": 5,
      "message": "`value` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `value` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
Price :: u16
main :: fn () => Price
¬
{
  "message": "Cannot use type `Price` as a value",
  "source_file": "tests/errors/bindings.e",
  "primary_location": {
    "line": 2,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 18,
      "length": 5,
      "message": "This name refers to a type"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/bindings.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Price` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Use `Price` in a type annotation or bind a runtime value instead."
  ]
}
