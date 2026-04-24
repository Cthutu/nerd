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
        "line": 8,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
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
    "message": "Type mismatch: expected `all plex fields`, found `different field count`",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 7,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
            "column": 16,
            "length": 1,
            "message": "This expression has type `different field count`"
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
    p :: Point { x: 1, x: 2 }
    prn($"{p.x}")
}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `unique plex field`, found `x`",
    "source_file": "tests/errors/028-plexes.e",
    "primary_location": {
        "line": 7,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
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
        "line": 7,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 7,
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
