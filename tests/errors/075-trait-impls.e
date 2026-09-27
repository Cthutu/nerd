Display :: trait {
    show :: fn (self: Self) -> string
    describe :: fn (self: Self) -> string
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
    "message": "Trait implementation is missing required members",
    "source_file": "tests/errors/075-trait-impls.e",
    "primary_location": {
        "line": 11,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 11,
            "column": 1,
            "length": 4,
            "message": "This implementation does not define every member required by `Display`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/075-trait-impls.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Display` is defined here"
        }
    ],
    "notes": [
        "Missing members: `show`, `describe`"
    ],
    "help": [
        "Add the missing members to this `impl` block."
    ]
}
