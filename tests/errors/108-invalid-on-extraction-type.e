main :: fn () {
    on yes => [value] {
        _ := value
    }
}
¬
{
    "message": "Cannot extract an `on` payload from `bool`",
    "source_file": "tests/errors/108-invalid-on-extraction-type.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 5,
            "message": "This binder requires a success or error payload"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 8,
            "length": 3,
            "message": "This expression has type `bool`"
        }
    ],
    "notes": [],
    "help": [
        "Remove the extraction binder to branch on this value, or match an optional or result value that carries a payload."
    ]
}
