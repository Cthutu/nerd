main :: fn () {
    value := 1
    value := 2
    return value
}
¬
{
    "message": "Duplicate binding for symbol `value`",
    "source_file": "tests/errors/010-scoped-locals.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 5,
            "message": "This binding redefines `value`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 5,
            "length": 5,
            "message": "Previous binding of `value` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
¬
main :: fn () {
    value := later
    later := 1
    return value
}
¬
{
    "message": "Unknown symbol `later`",
    "source_file": "tests/errors/010-scoped-locals.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 5,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `later` or fix the spelling."
    ]
}
¬
main :: fn () {
    value := value + 1
    return value
}
¬
{
    "message": "Unknown symbol `value`",
    "source_file": "tests/errors/010-scoped-locals.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 5,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `value` or fix the spelling."
    ]
}
¬
main :: fn () {
    {
        inner := 1
    }
    return inner
}
¬
{
    "message": "Unknown symbol `inner`",
    "source_file": "tests/errors/010-scoped-locals.e",
    "primary_location": {
        "line": 5,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 12,
            "length": 5,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `inner` or fix the spelling."
    ]
}
