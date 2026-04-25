value: i32 = 2
main :: fn () => on value {
    1 on value => 1
    else => 2
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `bool`, found `i32`",
    "source_file": "tests/errors/031-on-pattern-guards.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 5,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
value: i32 = 2
main :: fn () => on value {
    1 on no => 1
}
¬
{
    "code": "0327",
    "message": "Value-producing block-form `on` expressions must be exhaustive",
    "source_file": "tests/errors/031-on-pattern-guards.e",
    "primary_location": {
        "line": 2,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 18,
            "length": 2,
            "message": "This `on` can miss a value"
        }
    ],
    "notes": [],
    "help": [
        "Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
    ]
}
