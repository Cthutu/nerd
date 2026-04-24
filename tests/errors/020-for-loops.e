main :: fn () {
    for 1
}
¬
{
    "code": "0203",
    "message": "Expected LeftBrace `{`",
    "source_file": "tests/errors/020-for-loops.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
            "message": "Found Integer here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
