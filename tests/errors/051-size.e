io :: use std.io
value :: io.size
main :: fn () {}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `runtime-sized value`, found `module`",
    "source_file": "tests/errors/051-size.e",
    "primary_location": {
        "line": 2,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 13,
            "length": 4,
            "message": "This expression has type `module`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
