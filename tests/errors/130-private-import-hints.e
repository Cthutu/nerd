use private_hint_a
use private_hint_b
main :: fn () { _value: VertexArray }
¬
{
    "message": "Unknown type `VertexArray`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 3,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 25,
            "length": 11,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types.",
        "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?",
        "Used module `private_hint_b` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
    ]
}
¬
use private_hint_a
use private_hint_b
main :: fn () { _value := secret_value }
¬
{
    "message": "Unknown symbol `secret_value`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 3,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 27,
            "length": 12,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `secret_value` or fix the spelling.",
        "Used module `private_hint_a` declares `secret_value` privately. Did you intend to mark that declaration `pub`?",
        "Used module `private_hint_b` declares `secret_value` privately. Did you intend to mark that declaration `pub`?"
    ]
}
¬
a :: use private_hint_a
b :: use private_hint_a
main :: fn () { _value: VertexArray }
¬
{
    "message": "Unknown type `VertexArray`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 3,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 25,
            "length": 11,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types.",
        "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
    ]
}
¬
use private_hint_a
main :: fn () { _value: Hidden }
¬
{
    "message": "Unknown type `Hidden`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 2,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 25,
            "length": 6,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
¬
use private_hint_a
main :: fn () { _value: ValueOnly }
¬
{
    "message": "Unknown type `ValueOnly`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 2,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 25,
            "length": 9,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
¬
a :: use private_hint_a
main :: fn () { _value: a.VertexArray }
¬
{
    "message": "Unknown type `VertexArray`",
    "source_file": "tests/errors/130-private-import-hints.e",
    "primary_location": {
        "line": 2,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 27,
            "length": 11,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types.",
        "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
    ]
}
