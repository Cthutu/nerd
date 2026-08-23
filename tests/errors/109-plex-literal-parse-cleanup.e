main :: fn () {
    value := {
        first: 1
        second = 2
    }
    _ := value
}
¬
{
    "message": "Expected Symbol but found Equal `=`",
    "source_file": "tests/errors/109-plex-literal-parse-cleanup.e",
    "primary_location": {
        "line": 4,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 16,
            "length": 1,
            "message": "Found Equal `=` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
