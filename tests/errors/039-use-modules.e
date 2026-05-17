value :: 1
use value

main :: fn() {}
¬
{
    "message": "Type mismatch: expected `known module`, found `module path`",
    "source_file": "tests/errors/039-use-modules.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 5,
            "message": "This expression has type `module path`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
