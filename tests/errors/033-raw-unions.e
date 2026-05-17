Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { i: 1, f: 2.0 }
}¬
{
    "message": "Type mismatch: expected `one union field`, found `different field count`",
    "source_file": "tests/errors/033-raw-unions.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 1,
            "message": "This expression has type `different field count`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬Value :: union { i i32 f f32 }
main :: fn () -> i32 {
    v: Value = Value { i: 1 }
    return on v {
        else => 0
    }
}¬
{
    "message": "Block-form `on` does not support values of type `Value`",
    "source_file": "tests/errors/033-raw-unions.e",
    "primary_location": {
        "line": 4,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 15,
            "length": 1,
            "message": "This matched value type is unsupported"
        }
    ],
    "notes": [],
    "help": [
        "Block-form `on` supports `bool` and `string` scrutinees, plus concrete integer scrutinees."
    ]
}
¬Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value {}
}¬
{
    "message": "Type mismatch: expected `one union field`, found `different field count`",
    "source_file": "tests/errors/033-raw-unions.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 1,
            "message": "This expression has type `different field count`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { text: "bad" }
}¬
{
    "message": "Type mismatch: expected `known union field`, found `text`",
    "source_file": "tests/errors/033-raw-unions.e",
    "primary_location": {
        "line": 3,
        "column": 30
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 30,
            "length": 5,
            "message": "This expression has type `text`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬Value :: union { i i32 f f32 }
main :: fn () {
    v: Value = Value { i: 1 }
    w := v with { f: 2.0 }
}¬
{
    "message": "Type mismatch: expected `plex value`, found `Value`",
    "source_file": "tests/errors/033-raw-unions.e",
    "primary_location": {
        "line": 4,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 12,
            "length": 4,
            "message": "This expression has type `Value`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
