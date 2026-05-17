"Hello, world!
¬
{
    "message": "Unterminated string literal",
    "source_file": "tests/errors/006-unterminated-strings.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 14,
            "message": "String literal starts here"
        }
    ],
    "notes": [],
    "help": [
        "Add a closing double quote to terminate the string literal.",
        "Use `+\"...\"` for an intentional string continuation."
    ]
}
