use std.io

main :: fn () {
    text :: "hello"
    prn($"{text.len}")
}
¬
{
    "message": "Type mismatch: expected `string field `.data`, `.count`, or `.bytes``, found `len`",
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
