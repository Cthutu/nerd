main :: fn () {
    owner := box[u8](4)
    view: []i32 = owner
}

¬
{
    "message": "Type mismatch: expected `[]i32`, found `box[u8]`",
    "source_file": "tests/errors/151-implicit-box-borrows.e",
    "primary_location": {
        "line": 3,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 19,
            "length": 5,
            "message": "This expression has type `box[u8]`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
main :: fn () {
    owner := box[u8](4)
    pointer: ^i32 = owner
}

¬
{
    "message": "Type mismatch: expected `^i32`, found `box[u8]`",
    "source_file": "tests/errors/151-implicit-box-borrows.e",
    "primary_location": {
        "line": 3,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 21,
            "length": 5,
            "message": "This expression has type `box[u8]`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Pair :: [2]i32
main :: fn () {
    owner := box[Pair](2)
    flat: []i32 = owner
}

¬
{
    "message": "Type mismatch: expected `[]i32`, found `box[[2]i32]`",
    "source_file": "tests/errors/151-implicit-box-borrows.e",
    "primary_location": {
        "line": 4,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 19,
            "length": 5,
            "message": "This expression has type `box[[2]i32]`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
