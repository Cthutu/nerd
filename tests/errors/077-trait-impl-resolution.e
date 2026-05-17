Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Printable for Point {
    show :: fn (self: Self) => "point"
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `known trait`, found `Printable`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 9,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 1,
            "length": 4,
            "message": "This expression has type `Printable`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

impl Display for Missing {
    show :: fn (self: Self) => "missing"
}

main :: fn () => 0
¬
{
    "message": "Unknown type `Missing`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 5,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 18,
            "length": 7,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) => "first"
}

impl Display for Point {
    show :: fn (self: Self) => "second"
}

main :: fn () => 0
¬
{
    "message": "Duplicate binding for symbol `Display for Point`",
    "source_file": "tests/errors/077-trait-impl-resolution.e",
    "primary_location": {
        "line": 13,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 13,
            "column": 1,
            "length": 4,
            "message": "This binding redefines `Display for Point`"
        },
        {
            "kind": "secondary",
            "line": 9,
            "column": 1,
            "length": 4,
            "message": "Previous binding of `Display for Point` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}
