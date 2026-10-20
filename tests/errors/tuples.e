main :: fn () {
    value :: 1
    return value.0
}
¬
{
  "message": "Type mismatch: expected `tuple`, found `untyped integer`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 3,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 18,
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
    pair :: (1, "one")
    return pair.2
}
¬
{
  "message": "Type mismatch: expected `valid tuple field`, found `(i32, string)`",
  "source_file": "tests/errors/tuples.e",
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
      "message": "This expression has type `(i32, string)`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
pair: (i32, string): (1, 2)
main :: fn () => pair.0
¬
{
  "message": "Type mismatch: expected `string`, found `untyped integer`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 1,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 26,
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
    (a, b) := (1, 2, 3)
    prn($"{a} {b}")
}
¬
{
  "message": "Type mismatch: expected `tuple with matching arity`, found `(i32, i32, i32)`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 4,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 5,
      "length": 1,
      "message": "This expression has type `(i32, i32, i32)`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    (a, b) := 1
    prn($"{a} {b}")
}
¬
{
  "message": "Type mismatch: expected `tuple`, found `i32`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 1,
      "message": "This expression has type `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    (a, b) = (1, 2)
    prn($"{a} {b}")
}
¬
{
  "message": "Cannot assign to `a`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 2,
    "column": 6
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 6,
      "length": 1,
      "message": "`a` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `a` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
main :: fn () {
    a := 1
    b := "two"
    (a, b) = ("one", 2)
    prn($"{a} {b}")
}
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 4,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 15,
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
Point :: plex {
    x i32
}

main :: fn () {
    { y } := Point { x: 1 }
    prn($"{y}")
}
¬
{
  "message": "Type mismatch: expected `known plex field`, found `y`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 6,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 5,
      "length": 1,
      "message": "This expression has type `y`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
pair :: fn () -> (i32, i32) {
    return (2, 3)
}

main :: fn () {
    a, b := pair()
}
¬
{
  "message": "Tuple destructuring bindings must be parenthesized",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 6,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 5,
      "length": 4,
      "message": "bare tuple destructuring target starts here"
    }
  ],
  "notes": [
    "Comma-separated binding targets are parsed as destructuring only when they are wrapped in parentheses"
  ],
  "help": [
    "Use `(a, b) := value` instead of `a, b := value`"
  ]
}


¬
main :: fn () {
    pair :: (1, 2)
    pair.0 = 3
}
¬
{
  "message": "Cannot assign to tuple field rooted in immutable `pair`",
  "source_file": "tests/errors/tuples.e",
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
      "message": "This tuple field is part of immutable storage"
    }
  ],
  "notes": [],
  "help": [
    "Use a mutable binding or pass a pointer to mutable tuple storage."
  ]
}

¬
mutate :: fn (pair: (i32, i32)) { pair.0 += 1 }
main :: fn () { mutate((1, 2)) }
¬
{
  "message": "Cannot assign to tuple field rooted in immutable `pair`",
  "source_file": "tests/errors/tuples.e",
  "primary_location": {
    "line": 1,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 40,
      "length": 1,
      "message": "This tuple field is part of immutable storage"
    }
  ],
  "notes": [],
  "help": [
    "Use a mutable binding or pass a pointer to mutable tuple storage."
  ]
}
