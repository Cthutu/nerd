use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    prn($"{p.z}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `known plex field`, found `z`",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 10,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 14,
            "length": 1,
            "message": "This expression has type `z`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1 }
    prn($"{p.x}")
}
¬
{
    "code": "0304",
    "message": "Plex literal is missing required field",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 9,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 16,
            "length": 1,
            "message": "This literal does not initialise every required field"
        }
    ],
    "notes": [
        "Missing field: `y`"
    ],
    "help": [
        "Add all fields required by the plex type, or write `...` in the literal to zero-initialise omitted fields."
    ]
}
¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, x: 2 }
    prn($"{p.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `unique plex field`, found `x`",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 9,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 27,
            "length": 1,
            "message": "This expression has type `x`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use std.io

Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: "one", y: 2 }
    prn($"{p.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 9,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 21,
            "length": 5,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
