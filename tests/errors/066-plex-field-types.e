Bad :: plex {
    field ^Missing
}
¬
{
    "code": "0303",
    "message": "Unknown type `Missing`",
    "source_file": "tests/errors/066-plex-field-types.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 7,
            "message": "This type name is not defined"
        }
    ],
    "notes": [],
    "help": [
        "Use one of the built-in primitive types supported by the current milestone."
    ]
}
