(1+2
¬
{
    "code": "0203",
    "message": "Expected RightParen ())",
    "source_file": "tests/errors/003-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 5,
            "length": 0,
            "message": "Found EOF here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
1)
¬
{
    "code": "0204",
    "message": "Unexpected token after expression",
    "source_file": "tests/errors/003-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 2
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 2,
            "length": 1,
            "message": "Found RightParen ()) here"
        }
    ],
    "notes": [],
    "help": [
        "Remove the extra token or add an operator to continue the expression"
    ]
}
