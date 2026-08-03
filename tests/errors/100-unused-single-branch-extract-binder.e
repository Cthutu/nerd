Failure :: enum {
    Bad
}

get :: fn () -> i32\Failure {
    return 1
}

main :: fn () {
    on get() => [value] {
    }
}
¬
{
    "message": "Unused pattern binder `value`",
    "source_file": "tests/errors/100-unused-single-branch-extract-binder.e",
    "primary_location": {
        "line": 10,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 10,
            "column": 18,
            "length": 5,
            "message": "This pattern binder is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `value` or prefix the name with `_` if it is deliberately unused."
    ]
}
