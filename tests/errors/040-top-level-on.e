on 1 {}
¬
{
    "message": "Expected String but found Integer",
    "source_file": "tests/errors/040-top-level-on.e",
    "primary_location": {
        "line": 1,
        "column": 4
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 4,
            "length": 1,
            "message": "Found Integer here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
on !"debug" {
    answer :: 7
}

main :: fn () => answer
¬
{
    "message": "Unknown symbol `answer`",
    "source_file": "tests/errors/040-top-level-on.e",
    "primary_location": {
        "line": 5,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 18,
            "length": 6,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `answer` or fix the spelling."
    ]
}
