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
¬
Bad :: plex {
    field: i32
}
main :: fn () {}
¬
{
    "code": "0208",
    "message": "Expected type but found Colon `:`",
    "source_file": "tests/errors/066-plex-field-types.e",
    "primary_location": {
        "line": 2,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 10,
            "length": 1,
            "message": "A type is expected here"
        }
    ],
    "notes": [
        "Plex field definitions are written as `field Type`."
    ],
    "help": [
        "Remove the colon. Colons are used in plex literals such as `State { loc_index: 0 }`, not in plex definitions."
    ]
}
