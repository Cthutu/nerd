Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { z: 3 }
    prn($"{q.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `known plex field`, found `z`",
    "source_file": "tests/errors/029-plex-ergonomics.e",
    "primary_location": {
        "line": 8,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
            "column": 22,
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
Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { x: 3, x: 4 }
    prn($"{q.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `unique plex field`, found `x`",
    "source_file": "tests/errors/029-plex-ergonomics.e",
    "primary_location": {
        "line": 8,
        "column": 28
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
            "column": 28,
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
Point :: plex {
    x i32
    y i32
}

main :: fn () {
    p :: Point { x: 1, y: 2 }
    q := p with { x: "wrong" }
    prn($"{q.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/029-plex-ergonomics.e",
    "primary_location": {
        "line": 8,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
            "column": 22,
            "length": 7,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
    x := 1
    y := x with { value: 2 }
    prn($"{y}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `plex value`, found `i32`",
    "source_file": "tests/errors/029-plex-ergonomics.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 4,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
