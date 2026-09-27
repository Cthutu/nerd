Pair :: plex [T] {
    first T
}

Bad :: plex {
    use Pair[i32]
    use Pair[f32]
}

main :: fn () {}
¬
{
    "message": "Plex type `Pair` is embedded more than once",
    "source_file": "tests/errors/096-duplicate-generic-plex-use.e",
    "primary_location": {
        "line": 7,
        "column": 5
    },
    "references": [
        {
            "kind": "secondary",
            "source_file": "tests/errors/096-duplicate-generic-plex-use.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Pair` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Each plex may use a generic type declaration only once."
    ]
}
