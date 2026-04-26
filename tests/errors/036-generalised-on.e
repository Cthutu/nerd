main :: fn () -> i32 {
    value := 2
    return on {
        value < 0 => -1
        value > 0 => 1
    }
}
¬
{
    "code": "0327",
    "message": "Value-producing block-form `on` expressions must be exhaustive",
    "source_file": "tests/errors/036-generalised-on.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
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
main :: fn () -> i32 {
    value := 2
    return on {
        value => 1
        else => 0
    }
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `bool`, found `i32`",
    "source_file": "tests/errors/036-generalised-on.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
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
main :: fn () -> i32 {
    value := "hi"
    return on value {
        > "bye" => 1
        else => 0
    }
}
¬
{
    "code": "0326",
    "message": "Operator `>` requires matching numeric operands, found `string` and `string`",
    "source_file": "tests/errors/036-generalised-on.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 1,
            "message": "These operands have types `string` and `string`"
        }
    ],
    "notes": [],
    "help": [
        "Use `>` only with matching numeric operands."
    ]
}
