message :: 3.14
main :: fn () => on message {
    else => 1
}
¬
{
    "message": "Block-form `on` does not support values of type `untyped float`",
    "source_file": "tests/errors/016-on-value-branches.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 7,
            "message": "This matched value type is unsupported"
        }
    ],
    "notes": [],
    "help": [
        "Block-form `on` supports `bool` and `string` scrutinees, plus concrete integer scrutinees."
    ]
}
¬
value: i32 = 2
main :: fn () => on value {
    _ => 10
}
¬
{
    "message": "Block-form `on` wildcard pattern must use `else`",
    "source_file": "tests/errors/016-on-value-branches.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 1,
            "message": "This wildcard matches every value"
        }
    ],
    "notes": [],
    "help": [
        "Use an `else` branch instead of `_` as a top-level value pattern."
    ]
}
¬
