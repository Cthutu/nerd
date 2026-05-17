count := 3
message := $"count={count}"
¬
{
    "code": "0310",
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
    "code": "0311",
    "message": "Cannot interpolate values of type `fn () -> i32`",
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
            "message": "This expression type cannot be converted to string yet"
        }
    ],
    "notes": [],
    "help": [
        "Use a built-in primitive or `string`, or cast the value to a supported type first."
    ]
}
