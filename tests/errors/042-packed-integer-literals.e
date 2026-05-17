'abc
¬
{
    "message": "Unterminated packed integer literal",
    "source_file": "tests/errors/042-packed-integer-literals.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Packed integer literal starts here"
        }
    ],
    "notes": [],
    "help": [
        "Add a closing single quote to terminate the literal."
    ]
}
¬
'abcdefghi'
¬
{
    "message": "Packed integer literal is too large",
    "source_file": "tests/errors/042-packed-integer-literals.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 10,
            "message": "This literal exceeds the 8-byte limit"
        }
    ],
    "notes": [],
    "help": [
        "Use at most 8 bytes inside one single-quote literal."
    ]
}
