nothing :: fn () {}
main :: fn () {
    pointer := ^nothing()
}

¬
{
    "message": "Type mismatch: expected `addressable value`, found `void`",
    "source_file": "tests/errors/025-pointers.e",
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
    "source_file": "tests/errors/025-pointers.e",
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
    "source_file": "tests/errors/025-pointers.e",
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
main :: fn () { pointer := ^nil }
¬
{
    "message": "Type mismatch: expected `addressable value`, found `nil`",
    "source_file": "tests/errors/025-pointers.e",
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
    "source_file": "tests/errors/025-pointers.e",
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
