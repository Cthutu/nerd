main :: fn () {
    assert 1
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `bool`, found `untyped integer`",
    "source_file": "tests/errors/050-assert.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    assert yes, 1
}
¬
{
    "code": "0203",
    "message": "Expected String but found Integer",
    "source_file": "tests/errors/050-assert.e",
    "primary_location": {
        "line": 2,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 17,
            "length": 1,
            "message": "Found Integer here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
