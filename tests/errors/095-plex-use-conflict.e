Point :: plex {
    x i32
    y i32
}

Rect :: plex {
    x u32
    use Point
}

main :: fn () {}
¬
{
    "message": "Embedded plex field `x` conflicts with another field",
    "source_file": "tests/errors/095-plex-use-conflict.e",
    "primary_location": {
        "line": 8,
        "column": 5
    },
    "references": [],
    "notes": [],
    "help": [
        "Rename the field or remove one of the conflicting plex uses."
    ]
}
