Mode :: enum {
    Fixed {
        width  u16
        height u16
    }
}

main :: fn () {
    mode := Mode.Fixed(400, 300)
}
¬
{
    "message": "Enum variant with named fields requires braced syntax",
    "source_file": "tests/errors/094-braced-enum-payload-call-syntax.e",
    "primary_location": {
        "line": 9,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 23,
            "length": 1,
            "message": "This variant has a braced payload with named fields"
        }
    ],
    "notes": [],
    "help": [
        "Construct it as `Fixed { field: value }` instead of calling it with `(...)`."
    ]
}
