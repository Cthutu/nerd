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
        "Add a binding for `mystery` or fix the spelling."
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
¬
value: mystery: 1
¬
{
    "code": "0303",
    "message": "Unknown type `mystery`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 7,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use one of the built-in primitive types supported by the current milestone."
    ]
}
¬
value: string: 1
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `string`, found `untyped integer`",
    "source_file": "tests/errors/004-semantics.e",
    "primary_location": {
        "line": 1,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 16,
            "length": 1,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
