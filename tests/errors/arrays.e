use std.io

main :: fn () {
    value :: 1
    prn($"{value[0]}")
}
¬
{
  "message": "Type mismatch: expected `array, slice, dynamic array, string, or pointer`, found `untyped integer`",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This expression has type `untyped integer`"
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
    values :: [1, 2]
    prn($"{values["zero"]}")
}
¬
{
  "message": "Type mismatch: expected `integer index`, found `string`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 5,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 19,
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
values: [2]i32: [1, "two"]
main :: fn () => values[0]
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 1,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
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
values: [3]i32: [1, 2]
main :: fn () => values[0]
¬
{
  "message": "Type mismatch: expected `[3]i32`, found `array with different length`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 1,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 17,
      "length": 1,
      "message": "This expression has type `array with different length`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
values: [2]i32: [1, 2, 3]
main :: fn () => values[0]
¬
{
  "message": "Type mismatch: expected `[2]i32`, found `array with different length`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 1,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 17,
      "length": 1,
      "message": "This expression has type `array with different length`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value :: 1
    slice :: value[..]
}
¬
{
  "message": "Type mismatch: expected `array, slice, dynamic array, or string`, found `untyped integer`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 19,
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
main :: fn () {
    values :: [1, 2, 3]
    slice :: values[yes..]
}
¬
{
  "message": "Type mismatch: expected `integer slice bound`, found `bool`",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This expression has type `bool`"
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
    values :: [1, 2, 3]
    slice :: values[..]
    prn($"{slice.len}")
}
¬
{
  "message": "Type mismatch: expected `slice field .data, .count, .bytes, or defined method`, found `len`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 6,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 18,
      "length": 3,
      "message": "This expression has type `len`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    values :: [1, 2, 3]
    slice :: values[..< values.count]
}
¬
{
  "message": "Unexpected `<` after `..`",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This `<` cannot appear after `..`"
    }
  ],
  "notes": [
    "Exclusive end ranges and slices use `..`; inclusive end forms use `..=`"
  ],
  "help": [
    "Remove the `<` and keep `..` for an exclusive end range or slice"
  ]
}

¬
use std.io

main :: fn () {
    text :: "hello"
    prn($"{text.c_string}")
}
¬
{
  "message": "Type mismatch: expected `string field .data, .count, .bytes, or defined method`, found `c_string`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 5,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 17,
      "length": 8,
      "message": "This expression has type `c_string`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/mods/core.n",
      "line": 231,
      "column": 5,
      "length": 8,
      "message": "Type `c_string` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    text :: "hello"
    slice :: text[no..]
}
¬
{
  "message": "Type mismatch: expected `integer slice bound`, found `bool`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 19,
      "length": 2,
      "message": "This expression has type `bool`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
RoomType :: enum { NONE }
Room :: plex {
    exits ["many"]RoomType
}
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `non-negative integer constant`, found `string`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 2,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 1,
      "length": 4,
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
    names: [..]string
    names.push(123)
}
¬
{
  "message": "Type mismatch: expected `string`, found `untyped integer`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 16,
      "length": 3,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    names: [..]string
    names.append(123)
}
¬
{
  "message": "Type mismatch: expected `[]string`, found `untyped integer`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 18,
      "length": 3,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    names: [..]string
    names.pop("bad")
}
¬
{
  "message": "Argument count mismatch: expected 0, found 1",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 0 arguments to match the function signature."
  ]
}

¬
main :: fn () {
    names: [..]string
    names.delete("bad")
}
¬
{
  "message": "Type mismatch: expected `usize`, found `string`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 18,
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
    names: [..]string
    names.swap_delete()
}
¬
{
  "message": "Argument count mismatch: expected 1, found 0",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 1 argument to match the function signature."
  ]
}

¬
main :: fn () { n := 2.5
values: [n]i32 }
¬
{
  "message": "Type mismatch: expected `integer array length`, found `f64`",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This expression has type `f64`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
n: i32 = 3
values: [n]i32
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 2,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 9,
      "length": 1,
      "message": "This expression has type `runtime array length`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
work :: fn (n: i32, values: [n]i32) {}
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 1,
    "column": 29
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 29,
      "length": 1,
      "message": "This expression has type `runtime array length`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { n := 2
values: [n]i32 = [1, 2] }
¬
{
  "message": "Type mismatch: expected `constant array length outside a local array declaration`, found `runtime array length`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 2,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 9,
      "length": 1,
      "message": "This expression has type `runtime array length`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { n := 2
values: [n]i32
values = nil }
¬
{
  "message": "Cannot assign to `runtime-sized fixed array; assign elements instead`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 1,
      "length": 6,
      "message": "`runtime-sized fixed array; assign elements instead` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `runtime-sized fixed array; assign elements instead` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
main :: fn () { n := 2
values: [n]i32
values.resize_to(3) }
¬
{
  "message": "Type mismatch: expected `slice field .data, .count, .bytes, or defined method`, found `resize_to`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 8
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 8,
      "length": 9,
      "message": "This expression has type `resize_to`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { n := 2
values: [n]i32
values.free() }
¬
{
  "message": "Type mismatch: expected `slice field .data, .count, .bytes, or defined method`, found `free`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 3,
    "column": 8
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 8,
      "length": 4,
      "message": "This expression has type `free`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
work :: fn (n: i32) -> []i32 { values: [n]i32
return values }
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `return value with storage that outlives this scope`, found `local runtime-sized array`",
  "source_file": "tests/errors/arrays.e",
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
      "message": "This expression has type `local runtime-sized array`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { n: i32 = undefined
values: [n]i32 }
¬
{
  "message": "Cannot read `n` before it has been assigned",
  "source_file": "tests/errors/arrays.e",
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
      "message": "`n` is read here"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 17,
      "length": 1,
      "message": "`n` is declared with `undefined` here"
    }
  ],
  "notes": [
    "Variables declared with `undefined` must be assigned before they are read."
  ],
  "help": [
    "Assign to `n` on every path before using its value."
  ]
}


¬
main :: fn () { values := [1, 2]
 _ := values[0, 1] }
¬
{
  "message": "Type mismatch: expected `integer index`, found `(i32, i32)`",
  "source_file": "tests/errors/arrays.e",
  "primary_location": {
    "line": 2,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 13,
      "length": 1,
      "message": "This expression has type `(i32, i32)`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}
