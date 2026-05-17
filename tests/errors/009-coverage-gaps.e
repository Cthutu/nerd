18446744073709551616
¬
{
    "message": "Integer literal is too large",
    "source_file": "tests/errors/009-coverage-gaps.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 20,
            "message": "Literal overflow starts here"
        }
    ],
    "notes": [],
    "help": [
        "Use a smaller integer literal."
    ]
}
¬
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
¬
{
    "message": "Symbol is too long",
    "source_file": "tests/errors/009-coverage-gaps.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 256,
            "message": "This symbol is too long"
        }
    ],
    "notes": [
        "Symbols must be 255 characters or fewer."
    ],
    "help": [
        "Use a shorter symbol name."
    ]
}
¬
main ::
¬
{
    "message": "Expected declaration or expression but found EOF",
    "source_file": "tests/errors/009-coverage-gaps.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 0,
            "message": "Found EOF here"
        }
    ],
    "notes": [],
    "help": [
        "Expected a declaration or expression after '::', but found end of file"
    ]
}
