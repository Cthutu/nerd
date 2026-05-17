Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
}

impl Display for Point {
    show :: fn (self: Self) -> i32 {
        return self.x
    }
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `fn (Point) -> string`, found `fn (Point) -> i32`",
    "source_file": "tests/errors/076-trait-signatures.e",
    "primary_location": {
        "line": 10,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 5,
            "length": 4,
            "message": "This expression has type `fn (Point) -> i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
