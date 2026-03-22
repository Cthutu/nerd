+42
¬
{
    "code": "0201",
    "message": "Missing value before Plus `+`",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Plus `+` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
63+
¬
{
    "code": "0201",
    "message": "Missing value before EOF",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 4
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 4,
            "length": 0,
            "message": "EOF cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Insert a literal, parenthesized expression, or unary operator"
    ]
}
¬
28 29
¬
{
    "code": "0202",
    "message": "Missing operator before Integer",
    "source_file": "tests/errors/002-missing-values.e",
    "primary_location": {
        "line": 1,
        "column": 4
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 4,
            "length": 2,
            "message": "Integer starts a new expression here"
        }
    ],
    "notes": [],
    "help": [
        "Insert an operator such as +, -, *, /, or % between values"
    ]
}
