test {
    pub helper :: 42
}
¬
{
    "message": "Unexpected Keyword `pub` after expression",
    "source_file": "tests/errors/060-source-tests.e",
    "primary_location": {
        "line": 2,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 5,
            "length": 3,
            "message": "Found Keyword `pub` here"
        }
    ],
    "notes": [],
    "help": [
        "`pub` is not valid inside a test-only declaration block"
    ]
}
