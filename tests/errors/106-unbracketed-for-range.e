main :: fn () {
    for i in 0 .. 10 {
    }
}
¬
{
    "message": "Expected LeftBrace `{` but found Range `..`",
    "source_file": "tests/errors/106-unbracketed-for-range.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 2,
            "message": "Found Range `..` here"
        }
    ],
    "notes": [
        "Range expressions in `for` loops must be enclosed in square brackets"
    ],
    "help": [
        "Add square brackets around the range: `for item in [start .. end] { ... }`"
    ]
}
