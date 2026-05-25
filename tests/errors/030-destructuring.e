use std.io

main :: fn () {
    (a, b) := (1, 2, 3)
    prn($"{a} {b}")
}
¬
{
    "message": "Type mismatch: expected `tuple with matching arity`, found `(i32, i32, i32)`",
    "source_file": "tests/errors/030-destructuring.e",
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
    "source_file": "tests/errors/030-destructuring.e",
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
    "source_file": "tests/errors/030-destructuring.e",
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
    "source_file": "tests/errors/030-destructuring.e",
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
    "source_file": "tests/errors/030-destructuring.e",
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
