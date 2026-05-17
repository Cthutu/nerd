Private :: plex {
    value i32
}

pub Public :: plex {
    field Private
}

main :: fn () {}
¬
{
    "message": "Public type `Public` exposes private type `Private`",
    "source_file": "tests/errors/067-public-plex-private-fields.e",
    "primary_location": {
        "line": 6,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 11,
            "length": 7,
            "message": "`Private` is private to this module"
        }
    ],
    "notes": [
        "Public plex and union fields are part of the module's public API."
    ],
    "help": [
        "Make `Private` public or hide it behind a private implementation detail that is not stored in the public field list."
    ]
}
¬
Private :: plex {
    value i32
}

pub Public :: plex {
    field ^Private
}

main :: fn () {}
¬
{
    "message": "Public type `Public` exposes private type `Private`",
    "source_file": "tests/errors/067-public-plex-private-fields.e",
    "primary_location": {
        "line": 6,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 12,
            "length": 7,
            "message": "`Private` is private to this module"
        }
    ],
    "notes": [
        "Public plex and union fields are part of the module's public API."
    ],
    "help": [
        "Make `Private` public or hide it behind a private implementation detail that is not stored in the public field list."
    ]
}
