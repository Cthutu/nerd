Bad :: plex {
    field ^Missing
}
¬
{
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
        "Use a defined type name, or one of the built-in primitive types."
    ]
}
¬
Bad :: plex {
    field: i32
}
main :: fn () {}
¬
{
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
