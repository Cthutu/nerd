main :: fn () {
    _value: f64 = 1e
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 2,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e+
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 3,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e-
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 3,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e--3
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 3,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1E+-3
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 3,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e_3
}
¬
{
    "message": "Scientific exponent requires decimal digits",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 20,
            "length": 2,
            "message": "Expected digits after exponent marker and optional sign"
        }
    ],
    "notes": [],
    "help": [
        "Use forms such as `1e3`, `1.0e-3` or `1E+3`."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e3e2
}
¬
{
    "message": "Invalid character 'e' in number literal",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 19,
            "length": 4,
            "message": "Invalid character 'e' in number literal"
        }
    ],
    "notes": [],
    "help": [
        "Review the number literal for typos or insert whitespace."
    ]
}
¬
main :: fn () {
    _value: f64 = 1e3_
}
¬
{
    "message": "Invalid character '_' in number literal",
    "source_file": "tests/errors/152-scientific-floats.e",
    "primary_location": {
        "line": 2,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 19,
            "length": 4,
            "message": "Invalid character '_' in number literal"
        }
    ],
    "notes": [],
    "help": [
        "Review the number literal for typos or insert whitespace."
    ]
}
