(1+2
¬
{
    "code": "0203",
    "message": "Expected RightParen `)` but found EOF",
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
    "message": "Unexpected RightParen `)` after expression",
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
            "message": "Found RightParen `)` here"
        }
    ],
    "notes": [
        "This right parenthesis does not match an opening parenthesis"
    ],
    "help": [
        "Add the missing opening parenthesis or remove the extra right parenthesis"
    ]
}
¬
main :: fn () {
    return 0
¬
{
    "code": "0203",
    "message": "Expected RightBrace `}` but found EOF",
    "source_file": "tests/errors/003-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 15,
            "length": 1,
            "message": "This opening delimiter is not closed"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 13,
            "length": 0,
            "message": "Reached EOF while looking for RightBrace `}`"
        }
    ],
    "notes": [],
    "help": [
        "Add the missing closing delimiter for this block"
    ]
}
¬
impl Display for Point {
    show :: fn (self: Self) => "point"

Display :: trait {
    show :: fn (Self) -> string
}
¬
{
    "code": "0203",
    "message": "Expected RightBrace `}` before declaration",
    "source_file": "tests/errors/003-delimiters.e",
    "primary_location": {
        "line": 4,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 1,
            "length": 7,
            "message": "A closing delimiter is needed before this declaration"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 24,
            "length": 1,
            "message": "This opening delimiter is not closed"
        }
    ],
    "notes": [],
    "help": [
        "Add the missing closing delimiter before this line"
    ]
}
