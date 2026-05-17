bad :: use std.missing

main :: fn() {}¬
{
    "message": "Type mismatch: expected `known module`, found `module path`",
    "source_file": "tests/errors/038-modules.e",
    "primary_location": {
        "line": 1,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 12,
            "length": 3,
            "message": "This expression has type `module path`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
