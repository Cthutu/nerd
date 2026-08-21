main :: fn () {
    n system.poll(^frame) {
        None => 0
    }
}
¬
{
    "message": "Unexpected `n` before match expression",
    "source_file": "tests/errors/107-misspelled-on.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 1,
            "message": "`n` is parsed as a separate expression statement"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 7,
            "length": 6,
            "message": "This looks like the value to match"
        }
    ],
    "notes": [],
    "help": [
        "Replace `n` with `on`"
    ]
}
