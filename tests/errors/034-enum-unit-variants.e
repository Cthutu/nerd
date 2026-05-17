main :: fn () {
    value :: Red
}
¬
{
    "message": "Unknown symbol `Red`",
    "source_file": "tests/errors/034-enum-unit-variants.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `Red` or fix the spelling."
    ]
}
¬
Colour :: enum { Red Green }

main :: fn () {
    value: Colour = Blue
}
¬
{
    "message": "Type mismatch: expected `known enum variant`, found `Blue`",
    "source_file": "tests/errors/034-enum-unit-variants.e",
    "primary_location": {
        "line": 4,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 21,
            "length": 4,
            "message": "This expression has type `Blue`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Red Green Blue }

main :: fn () -> i32 {
    value: Colour = Red
    return on value {
        Red => 1
        Green => 2
    }
}
¬
{
    "message": "Value-producing block-form `on` expressions must be exhaustive",
    "source_file": "tests/errors/034-enum-unit-variants.e",
    "primary_location": {
        "line": 5,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 12,
            "length": 2,
            "message": "This `on` does not produce a value on every path"
        }
    ],
    "notes": [],
    "help": [
        "Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
    ]
}
¬
Colour :: enum { Red Green }

main :: fn () {
    value := Colour.Blue
}
¬
{
    "message": "Type mismatch: expected `known enum variant`, found `Blue`",
    "source_file": "tests/errors/034-enum-unit-variants.e",
    "primary_location": {
        "line": 4,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 21,
            "length": 4,
            "message": "This expression has type `Blue`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
