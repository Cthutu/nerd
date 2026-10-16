value :: true
main :: fn () {}
¬
{
    "message": "Unknown symbol `true`",
    "source_file": "tests/errors/148-boolean-literal-spellings.e",
    "primary_location": {
        "line": 1,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 10,
            "length": 4,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `true` or fix the spelling."
    ]
}

¬
value :: false
main :: fn () {}
¬
{
    "message": "Unknown symbol `false`",
    "source_file": "tests/errors/148-boolean-literal-spellings.e",
    "primary_location": {
        "line": 1,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 10,
            "length": 5,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `false` or fix the spelling."
    ]
}

¬
main :: fn () -> bool { return true }
¬
{
    "message": "Unknown symbol `true`",
    "source_file": "tests/errors/148-boolean-literal-spellings.e",
    "primary_location": {
        "line": 1,
        "column": 32
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 32,
            "length": 4,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `true` or fix the spelling."
    ]
}

¬
main :: fn () { on false => prn("unexpected") }
¬
{
    "message": "Unknown symbol `false`",
    "source_file": "tests/errors/148-boolean-literal-spellings.e",
    "primary_location": {
        "line": 1,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 20,
            "length": 5,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `false` or fix the spelling."
    ]
}

