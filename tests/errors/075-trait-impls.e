Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
}

main :: fn () => 0
¬
{
    "message": "Type mismatch: expected `trait member`, found `show`",
    "source_file": "tests/errors/075-trait-impls.e",
    "primary_location": {
        "line": 10,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 1,
            "length": 4,
            "message": "This expression has type `show`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
