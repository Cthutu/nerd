TokenType :: enum {
    Identifier
}

impl Display for TokenType {
    debug :: fn (self: TokenType) -> string => "Identifier"
}

main :: fn () => 0
¬
{
    "message": "Trait implementation is missing required member",
    "source_file": "tests/errors/111-imported-trait-impl-members.e",
    "primary_location": {
        "line": 5,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 1,
            "length": 4,
            "message": "This implementation does not define every member required by `Display`"
        }
    ],
    "notes": [
        "Missing member: `show`"
    ],
    "help": [
        "Add the missing member to this `impl` block."
    ]
}
