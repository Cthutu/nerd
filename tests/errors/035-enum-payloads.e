Maybe :: enum { None Some(i32) }

main :: fn () {
    value: Maybe = Some("bad")
}¬
{
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
¬Event :: enum {
    Resized {
        width u16
        height u16
    }
}

main :: fn () {
    event := Event.Resized { width: 1, height: 2 }
    on event {
        Resized(width, height) => {}
    }
}¬
{
    "message": "Enum payload pattern is missing field names",
    "source_file": "tests/errors/035-enum-payloads.e",
    "primary_location": {
        "line": 11,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 11,
            "column": 9,
            "length": 7,
            "message": "This variant has a braced payload with named fields"
        }
    ],
    "notes": [],
    "help": [
        "Name the fields in the pattern, for example `Resized { field: binding }`. If the binding name is the same as the field name, `Resized { field }` is shorthand."
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
        Other.Some(x) => x
        else => 0
    }
}¬
{
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
¬Maybe :: enum { None Some(i32) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Some(x), None => x
    }
}¬
{
    "message": "Type mismatch: expected `non-payload enum variant`, found `Some`",
    "source_file": "tests/errors/035-enum-payloads.e",
    "primary_location": {
        "line": 6,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 9,
            "length": 4,
            "message": "This expression has type `Some`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
