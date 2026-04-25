main :: fn () {
    value :: .Red
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `enum context`, found `<unknown>`",
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
            "length": 1,
            "message": "This expression has type `<unknown>`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Red Green }

main :: fn () {
    value: Colour = .Blue
}
¬
{
    "code": "0304",
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
            "length": 1,
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
    value: Colour = .Red
    return on value {
        .Red => 1
        .Green => 2
    }
}
¬
{
    "code": "0327",
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
            "message": "This `on` can miss a value"
        }
    ],
    "notes": [],
    "help": [
        "Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
    ]
}
