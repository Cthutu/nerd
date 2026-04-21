main :: fn () => mystery
¬
{
    "code": "0300",
    "message": "Unknown symbol `mystery`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 18,
            "length": 7,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `mystery` before it is used or fix the spelling."
    ]
}
¬
answer :: 42
answer :: 7
¬
{
    "code": "0301",
    "message": "Duplicate binding for symbol `answer`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 6,
            "message": "This binding redefines `answer`"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Previous binding of `answer` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
