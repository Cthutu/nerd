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
    "references": [],
    "notes": [],
    "help": [
        "Each plex may use a generic type declaration only once."
    ]
}
