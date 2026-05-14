Bad :: plex {
    field MissingType
}
main :: fn () {}
¬
{
    "code": "0303",
    "message": "Unknown type `MissingType`",
    "source_file": "tests/errors/062-type-declarations.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 11,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
