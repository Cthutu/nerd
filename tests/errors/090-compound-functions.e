empty :: fn {}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `compound function with at least one member`, found `empty compound function`",
    "source_file": "tests/errors/090-compound-functions.e",
    "primary_location": {
        "line": 1,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 10,
            "length": 2,
            "message": "This expression has type `empty compound function`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
