main :: fn () { pr() }
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/136-pr-requires-text.e",
    "primary_location": {
        "line": 1,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 19,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass an argument for parameter `text`."
    ]
}
¬
main :: fn () { value := temp_arena.pr() }
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/136-pr-requires-text.e",
    "primary_location": {
        "line": 1,
        "column": 39
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 39,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass an argument for parameter `text`."
    ]
}
¬
main :: fn () { epr() }
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/136-pr-requires-text.e",
    "primary_location": {
        "line": 1,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 20,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass an argument for parameter `text`."
    ]
}
¬
main :: fn () { value := temp_arena.prn() }
¬
{
    "message": "Argument count mismatch: expected 1, found 0",
    "source_file": "tests/errors/136-pr-requires-text.e",
    "primary_location": {
        "line": 1,
        "column": 40
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 40,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass an argument for parameter `text`."
    ]
}
