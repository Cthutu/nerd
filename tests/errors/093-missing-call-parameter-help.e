Thing :: plex {}

impl Thing {
    pub poll :: fn (self: ^Self, frame: i32, flags: bool) {}
}

main :: fn () {
    thing: Thing = {}
    thing.poll()
}
¬
{
    "message": "Argument count mismatch: expected 2, found 0",
    "source_file": "tests/errors/093-missing-call-parameter-help.e",
    "primary_location": {
        "line": 9,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 15,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass an argument for parameter `frame`.",
        "Pass an argument for parameter `flags`."
    ]
}
