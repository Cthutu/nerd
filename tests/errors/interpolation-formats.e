main :: fn () {
 value := $"{42:}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 1,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{42:02Q}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 4,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{42:1025X}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 6,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{42:99999999999999d}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 16,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{42:.2f}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 4,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{42: 02X}"
}
¬
{
    "message": "Unexpected InterpolationFormat after expression",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 16,
            "length": 5,
            "message": "Found InterpolationFormat here"
        }
    ],
    "notes": [],
    "help": [
        "Use [0][width][d|b|o|x|X], with a width no greater than 1024."
    ]
}
¬
main :: fn () {
 value := $"{1.5:02X}"
}
¬
{
    "message": "Type mismatch: expected `integer for an interpolation format`, found `f64`",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This expression has type `f64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value := $"{yes:02X}"
}
¬
{
    "message": "Type mismatch: expected `integer for an interpolation format`, found `bool`",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This expression has type `bool`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value := $"{"text":02X}"
}
¬
{
    "message": "Type mismatch: expected `integer for an interpolation format`, found `string`",
    "source_file": "tests/errors/interpolation-formats.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 6,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
