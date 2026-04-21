main :: fn () {
    return
}
¬
{
    "code": "0201",
    "message": "Missing value before RightBrace `}`",
    "source_file": "tests/errors/007-return-missing-value.e",
    "primary_location": {
        "line": 3,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 1,
            "length": 1,
            "message": "RightBrace `}` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
