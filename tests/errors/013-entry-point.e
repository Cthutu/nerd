¬
{
    "code": "0315",
    "message": "Missing entry point `main`",
    "source_file": "tests/errors/013-entry-point.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 0,
            "message": "No `main` function is defined"
        }
    ],
    "notes": [
        "Programs currently require a `main` entry point."
    ],
    "help": [
        "Add `main :: fn () => 0` or another zero-parameter function returning `i32`."
    ]
}
¬
main :: 1
¬
{
    "code": "0316",
    "message": "Invalid type for entry point `main`: found `i32`",
    "source_file": "tests/errors/013-entry-point.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "`main` must be a zero-parameter function returning an integer"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` so it takes no parameters and returns an integer type."
    ]
}
