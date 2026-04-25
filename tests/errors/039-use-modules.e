value :: 1
use value

main :: fn() {}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `module`, found `untyped integer`",
    "source_file": "tests/errors/039-use-modules.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 3,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
