18446744073709551616
¬
{
    "code": "0101",
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
    "code": "0104",
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
    "code": "0205",
    "message": "Expected declaration or expression",
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
