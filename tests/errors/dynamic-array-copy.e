main :: fn () {
    source: [2]u32 = [1, 2]
    target: [..]i32 = source
}
¬
{
    "message": "Type mismatch: expected `[..]i32`, found `[2]u32`",
    "source_file": "tests/errors/dynamic-array-copy.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 6,
            "message": "This expression has type `[2]u32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    source: []u32 = [1, 2]
    target: [..]i32 = source
}
¬
{
    "message": "Type mismatch: expected `[..]i32`, found `[]u32`",
    "source_file": "tests/errors/dynamic-array-copy.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
            "length": 6,
            "message": "This expression has type `[]u32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
A :: plex { id u32 }
B :: plex { name string }
main :: fn () {
    source: [1]A = [{ id: 1 }]
    target: [..]B = source
}
¬
{
    "message": "Type mismatch: expected `[..]B`, found `[1]A`",
    "source_file": "tests/errors/dynamic-array-copy.e",
    "primary_location": {
        "line": 5,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 21,
            "length": 6,
            "message": "This expression has type `[1]A`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/dynamic-array-copy.e",
            "line": 2,
            "column": 1,
            "length": 1,
            "message": "Type `B` is defined here"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/dynamic-array-copy.e",
            "line": 1,
            "column": 1,
            "length": 1,
            "message": "Type `A` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    source: [..]i32 = [1, 2]
    target: [2]i32 = source
}
¬
{
    "message": "Type mismatch: expected `[2]i32`, found `[..]i32`",
    "source_file": "tests/errors/dynamic-array-copy.e",
    "primary_location": {
        "line": 3,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 22,
            "length": 6,
            "message": "This expression has type `[..]i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
