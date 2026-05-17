add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c
main :: fn () => add(a = 1, c = 3)
¬
{
    "message": "Named argument `c` does not match parameter `b`",
    "source_file": "tests/errors/064-named-call-arguments.e",
    "primary_location": {
        "line": 2,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 29,
            "length": 1,
            "message": "This argument is named `c`"
        }
    ],
    "notes": [
        "Named arguments are currently checked in parameter order."
    ],
    "help": [
        "Move `c = ...` to the matching parameter position or provide `b = ...` here."
    ]
}
