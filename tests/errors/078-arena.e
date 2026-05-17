main :: fn () {
    a := arena()
}
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/078-arena.e",
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
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 1 argument to match the function signature."
    ]
}
¬
main :: fn () {
    a := arena("bad")
}
¬
{
    "message": "Type mismatch: expected `usize`, found `string`",
    "source_file": "tests/errors/078-arena.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
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
use core

main :: fn () {
    a := arena(16)
    _ := a.alloc()
}
¬
{
    "message": "Type mismatch: expected `inferable generic type parameter`, found `T`",
    "source_file": "tests/errors/078-arena.e",
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
            "message": "This expression has type `T`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use core

main :: fn () {
    a := arena(16)
    _ := a.alloc_array[i32]("bad")
}
¬
{
    "message": "Type mismatch: expected `usize`, found `string`",
    "source_file": "tests/errors/078-arena.e",
    "primary_location": {
        "line": 5,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 29,
            "length": 5,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
