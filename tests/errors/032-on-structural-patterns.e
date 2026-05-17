main :: fn () => on (1, 2) {
    (1, 2, 3) => 1
    else => 0
}
¬
{
    "message": "Type mismatch: expected `tuple with matching arity`, found `(i32, i32)`",
    "source_file": "tests/errors/032-on-structural-patterns.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 1,
            "message": "This expression has type `(i32, i32)`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Point :: plex { x i32 y i32 }
main :: fn () => on Point { x: 1, y: 2 } {
    { z: 3 } => 1
    else => 0
}
¬
{
    "message": "Type mismatch: expected `known plex field`, found `z`",
    "source_file": "tests/errors/032-on-structural-patterns.e",
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
            "message": "This expression has type `z`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () => on (1, 2) {
    (1, x) => 1
    else => 0
}
¬
{
    "message": "Unknown symbol `x`",
    "source_file": "tests/errors/032-on-structural-patterns.e",
    "primary_location": {
        "line": 2,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 9,
            "length": 1,
            "message": "This symbol is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Add a binding for `x` or fix the spelling."
    ]
}
¬
Point :: plex { x i32 }
Other :: plex { x i32 }

main :: fn () -> i32 {
    point := Point { x: 1 }
    return on point {
        Other { x: 1 } => 1
        else => 0
    }
}
¬
{
    "message": "Type mismatch: expected `Point`, found `Other`",
    "source_file": "tests/errors/032-on-structural-patterns.e",
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
