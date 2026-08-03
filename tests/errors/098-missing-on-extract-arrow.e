get_result :: fn () -> i32\string {
    return 1
}

main :: fn () {
    on get_result() [value] {
        value
    }
}
¬
{
    "message": "Missing `=>` before `on` extraction binder",
    "source_file": "tests/errors/098-missing-on-extract-arrow.e",
    "primary_location": {
        "line": 6,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 21,
            "length": 6,
            "message": "This looks like a success payload binder"
        },
        {
            "kind": "secondary",
            "line": 8,
            "column": 5,
            "length": 1,
            "message": "The missing arrow made this parse as indexing"
        }
    ],
    "notes": [],
    "help": [
        "Write `on value => [payload] { ... } else [error] { ... }`"
    ]
}
¬
get_result :: fn () -> i32\string {
    return 1
}

main :: fn () {
    on get_result()[value] {
    }
}
¬
{
    "message": "Missing `=>` before `on` extraction binder",
    "source_file": "tests/errors/098-missing-on-extract-arrow.e",
    "primary_location": {
        "line": 6,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 20,
            "length": 6,
            "message": "This looks like a success payload binder"
        }
    ],
    "notes": [
        "Without `=>`, `[payload]` is parsed as indexing"
    ],
    "help": [
        "Write `on value => [payload] { ... } else [error] { ... }`"
    ]
}
