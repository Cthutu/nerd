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
