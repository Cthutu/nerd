use std.io

main :: fn () {
    text :: "hello"
    prn($"{text.c_string}")
}
¬
{
    "message": "Type mismatch: expected `string field .data, .count, .bytes, or defined method`, found `c_string`",
    "source_file": "tests/errors/027-string-slices.e",
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
    "source_file": "tests/errors/027-string-slices.e",
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
