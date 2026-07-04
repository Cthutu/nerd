main :: fn () {
    value :: 1
    slice :: value[..]
}
¬
{
    "message": "Type mismatch: expected `array, slice, dynamic array, or string`, found `untyped integer`",
    "source_file": "tests/errors/026-slices.e",
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
    "source_file": "tests/errors/026-slices.e",
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
    "source_file": "tests/errors/026-slices.e",
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
    "source_file": "tests/errors/026-slices.e",
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
