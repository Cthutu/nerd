Bad :: plex {
    field MissingType
}
main :: fn () {}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `type declaration`, found `non-type value in type declaration`",
    "source_file": "tests/errors/062-type-declarations.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 3,
            "message": "This expression has type `non-type value in type declaration`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
