¬
{
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
        "Add `main :: fn () => 0`, `main :: fn (args: []string) => 0`, or another supported function bound to main returning `i32` or no type at all."
    ]
}
¬
main :: 1
¬
{
    "message": "Invalid type for entry point `main`: found `untyped integer`",
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
            "message": "`main` must be a function with no parameters or one `[]string` parameter, returning `i32` or no value"
        }
    ],
    "notes": [],
    "help": [
        "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
    ]
}
