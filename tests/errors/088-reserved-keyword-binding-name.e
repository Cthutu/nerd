Thing :: plex {}

impl Thing {
    pub use :: fn (self: Self) {}
}

main :: fn () {}
¬
{
    "message": "Keyword `use` cannot be used as a binding name",
    "source_file": "tests/errors/088-reserved-keyword-binding-name.e",
    "primary_location": {
        "line": 4,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 9,
            "length": 3,
            "message": "`use` is reserved for the language grammar"
        }
    ],
    "notes": [],
    "help": [
        "Choose a non-keyword name such as `bind`, `activate`, or `use_shader`."
    ]
}
