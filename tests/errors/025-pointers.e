main :: fn () {
    value :: 1
    pointer :: ^value
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `addressable value`, found `untyped integer`",
    "source_file": "tests/errors/025-pointers.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 5,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use mod std.print

main :: fn () {
    values: [2]i32 = [1, 2]
    pointer: ^i32 = ^values[0]
    prn($"{pointer["zero"]}")
}
¬
{
    "code": "0304",
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
    "code": "0304",
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
