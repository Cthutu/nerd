Key :: enum { Q Escape }
Event :: enum { Press { key Key } }
main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q }, Press { key: ESC } => {}
    }
}
¬
{
    "message": "Type mismatch: expected `known enum variant`, found `ESC`",
    "source_file": "tests/errors/132-grouped-payload-patterns.e",
    "primary_location": {
        "line": 6,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 40,
            "length": 3,
            "message": "This expression has type `ESC`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
