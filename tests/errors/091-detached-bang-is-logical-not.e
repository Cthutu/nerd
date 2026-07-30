main :: fn () {
    while !yes {
    }
}
¬
{
    "message": "Unknown symbol `while`",
    "source_file": "tests/errors/091-detached-bang-is-logical-not.e",
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
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `while` or fix the spelling."
    ]
}
