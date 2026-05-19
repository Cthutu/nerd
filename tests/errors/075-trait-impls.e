Display :: trait {
    show :: fn (Self) -> string
    describe :: fn (Self) -> string
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
        }
    ],
    "notes": [
        "Missing members: `show`, `describe`"
    ],
    "help": [
        "Add the missing members to this `impl` block."
    ]
}
