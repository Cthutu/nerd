use std.raptor
wrong :: fn (value: ^i32) -> f64 { return value^.as(f64) }
main :: fn () {
    task : Task[i32]
    number : i32
    task.create(wrong, ^number)
}
¬
{
    "message": "Type mismatch: expected `generic parameter-compatible argument`, found `fn (^i32) -> f64`",
    "source_file": "tests/errors/142-raptor-task-preparation.e",
    "primary_location": {
        "line": 6,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 17,
            "length": 5,
            "message": "This expression has type `fn (^i32) -> f64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
use std.raptor
square :: fn (value: ^i32) -> i32 { return value^ * value^ }
main :: fn () {
    task : Task[i32]
    number : f64
    task.create(square, ^number)
}
¬
{
    "message": "Type mismatch: expected `^i32`, found `^f64`",
    "source_file": "tests/errors/142-raptor-task-preparation.e",
    "primary_location": {
        "line": 6,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 25,
            "length": 1,
            "message": "This expression has type `^f64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

