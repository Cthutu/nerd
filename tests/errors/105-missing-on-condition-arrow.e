main :: fn () -> bool {
    left := 1
    right := 2
    on left != right {
        return false
    }
    return true
}
¬
{
    "message": "Missing value before Keyword `return`",
    "source_file": "tests/errors/105-missing-on-condition-arrow.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 6,
            "message": "Keyword `return` cannot appear here"
        }
    ],
    "notes": [],
    "help": [
        "Add `=>` before the block: `on condition => { ... }`"
    ]
}
