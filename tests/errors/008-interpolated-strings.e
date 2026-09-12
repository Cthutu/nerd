count := 3
message := $"count={count}"
¬
{
    "message": "Runtime interpolated strings cannot be top-level values",
    "source_file": "tests/errors/008-interpolated-strings.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 2,
            "message": "This interpolation needs runtime string building"
        }
    ],
    "notes": [],
    "help": [
        "Use only compile-time values in top-level interpolated strings, or move the interpolation into a function."
    ]
}
¬
use std.io

helper :: fn () => 1
main :: fn () {
    prn($"helper={helper}")
}
¬
{
    "message": "Type mismatch: expected `Display implementation`, found `fn () -> i32`",
    "source_file": "tests/errors/008-interpolated-strings.e",
    "primary_location": {
        "line": 5,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 19,
            "length": 6,
            "message": "This expression has type `fn () -> i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
