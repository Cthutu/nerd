message: string = $"Hello"
¬
{
    "code": "0310",
    "message": "Interpolated strings are only supported inside functions",
    "source_file": "tests/errors/008-interpolated-strings.e",
    "primary_location": {
        "line": 1,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 19,
            "length": 2,
            "message": "This interpolated string appears at top level"
        }
    ],
    "notes": [],
    "help": [
        "Move the interpolation inside a function body for the current milestone."
    ]
}
¬
use mod std.print

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
¬
main :: fn () => $"Hello"
¬
{
    "code": "0312",
    "message": "Interpolated string cannot escape statement scope",
    "source_file": "tests/errors/008-interpolated-strings.e",
    "primary_location": {
        "line": 1,
        "column": 18
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 18,
            "length": 2,
            "message": "This value would outlive the temporary string arena"
        }
    ],
    "notes": [],
    "help": [
        "Use interpolated strings only in statement-local contexts such as call arguments for now."
    ]
}
