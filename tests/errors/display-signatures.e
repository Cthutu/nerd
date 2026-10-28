World :: plex { count i32 }
impl Display for World {
    show :: fn (self: Self) => $"{self.count}"
}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `fn (^World) -> string`, found `fn (World) -> string`",
    "source_file": "tests/errors/display-signatures.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 4,
            "message": "This expression has type `fn (World) -> string`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/display-signatures.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `World` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
World :: plex { count i32 }
impl Display for World {
    show :: fn (self: ^Self) -> i32 { return self.count }
}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `fn (^World) -> string`, found `fn (^World) -> i32`",
    "source_file": "tests/errors/display-signatures.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 4,
            "message": "This expression has type `fn (^World) -> i32`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/display-signatures.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `World` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Cell :: plex [T] { value T }
impl [T] Display for Cell[T] {
    show :: fn (_self: Self) -> string { return "cell" }
}
main :: fn () { cell := Cell[i32] { value: 42 } prn($"{cell}") }
¬
{
    "message": "Type mismatch: expected `fn (^plex { i32 value }) -> string`, found `fn (plex { i32 value }) -> string`",
    "source_file": "tests/errors/display-signatures.e",
    "primary_location": {
        "line": 3,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 13,
            "length": 2,
            "message": "This expression has type `fn (plex { i32 value }) -> string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
use display_contracts
World :: plex { count i32 }
impl ByValue for World {
    render :: fn (self: ^Self) => $"{self.count}"
}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `fn (World) -> string`, found `fn (^World) -> string`",
    "source_file": "tests/errors/display-signatures.e",
    "primary_location": {
        "line": 4,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 5,
            "length": 6,
            "message": "This expression has type `fn (^World) -> string`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/display-signatures.e",
            "line": 2,
            "column": 1,
            "length": 5,
            "message": "Type `World` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}