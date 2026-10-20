value: i32 = 2
main :: fn () => on value {
    1 on value => 1
    else => 2
}
¬
{
    "message": "`on` condition requires bool, box, optional or result, found `i32`",
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
            "message": "This expression cannot be used as a condition"
        }
    ],
    "notes": [],
    "help": [
        "Use bool or box/optional/result presence, or block-form `on` for value matching."
    ]
}
¬
value: i32 = 2
main :: fn () => on value {
    1 on no => 1
}
¬
{
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
            "message": "This `on` does not produce a value on every path"
        }
    ],
    "notes": [],
    "help": [
        "Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
    ]
}
