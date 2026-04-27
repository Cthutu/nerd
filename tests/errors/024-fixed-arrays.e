use std.io

main :: fn () {
    value :: 1
    prn($"{value[0]}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `array, slice, dynamic array, string, or pointer`, found `untyped integer`",
    "source_file": "tests/errors/024-fixed-arrays.e",
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
    "code": "0304",
    "message": "Type mismatch: expected `integer index`, found `string`",
    "source_file": "tests/errors/024-fixed-arrays.e",
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
    "code": "0304",
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/024-fixed-arrays.e",
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
    "code": "0304",
    "message": "Type mismatch: expected `[3]i32`, found `array with different length`",
    "source_file": "tests/errors/024-fixed-arrays.e",
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
    "code": "0304",
    "message": "Type mismatch: expected `[2]i32`, found `array with different length`",
    "source_file": "tests/errors/024-fixed-arrays.e",
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
