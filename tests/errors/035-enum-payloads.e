Maybe :: enum { None Some(i32) }

main :: fn () {
    value: Maybe = Some("bad")
}¬
{
    "code": "0304",
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/035-enum-payloads.e",
    "primary_location": {
        "line": 4,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 25,
            "length": 5,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬Maybe :: enum { None Some(i32) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        None => 0
        Some(x, y) => x + y
    }
}¬
{
    "code": "0313",
    "message": "Argument count mismatch: expected 1, found 2",
    "source_file": "tests/errors/035-enum-payloads.e",
    "primary_location": {
        "line": 7,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
            "column": 9,
            "length": 4,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 1 argument to match the function signature."
    ]
}
¬Maybe :: enum { None Some(i32) }
Other :: enum { None Some(i64) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Other.Some(as x) => x
        else => 0
    }
}¬
{
    "code": "0304",
    "message": "Type mismatch: expected `Maybe`, found `Other`",
    "source_file": "tests/errors/035-enum-payloads.e",
    "primary_location": {
        "line": 7,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
            "column": 9,
            "length": 5,
            "message": "This expression has type `Other`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
