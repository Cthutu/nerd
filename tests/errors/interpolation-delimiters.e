main :: fn () { prn($"value={value") }
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "This interpolation expression starts here"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 35,
            "length": 1,
            "message": "Possible closing quote reached before the interpolation was complete"
        }
    ],
    "notes": [],
    "help": [
        "Add '}' to close the interpolation before the string's closing quote."
    ]
}
¬
main :: fn () { prn($"value={pick(value") }
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "This interpolation expression starts here"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 34,
            "length": 1,
            "message": "This '(' still needs a matching ')'"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 40,
            "length": 1,
            "message": "Possible closing quote reached before the interpolation was complete"
        }
    ],
    "notes": [],
    "help": [
        "Add ')', then '}', before the string's closing quote."
    ]
}
¬
main :: fn () { prn($"value={items[index") }
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "This interpolation expression starts here"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 35,
            "length": 1,
            "message": "This '[' still needs a matching ']'"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 41,
            "length": 1,
            "message": "Possible closing quote reached before the interpolation was complete"
        }
    ],
    "notes": [],
    "help": [
        "Add ']', then '}', before the string's closing quote."
    ]
}
¬
main :: fn () { prn($"value={value
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "This interpolation expression starts here"
        }
    ],
    "notes": [
        "Reached the end of the source before the interpolation was closed."
    ],
    "help": [
        "Add '}' to close the interpolation before the string's closing quote."
    ]
}
¬
plural :: fn (count: usize) => on count == 1 => "" else "s"
main :: fn () {
    count: usize = 1
    other: usize = 2
    prn($"{count} location{plural(count), {other} mob{plural(other)}>")
}
unrelated :: "Elwynn Forest"
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 5,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 27,
            "length": 1,
            "message": "This interpolation expression starts here"
        },
        {
            "kind": "secondary",
            "line": 5,
            "column": 70,
            "length": 1,
            "message": "Possible closing quote reached before the interpolation was complete"
        }
    ],
    "notes": [],
    "help": [
        "Add '}' to close the interpolation before the string's closing quote."
    ]
}
¬
main :: fn () { prn($"{ $"nested {value" }") }
¬
{
    "message": "Unterminated interpolation expression",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 34
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 34,
            "length": 1,
            "message": "This interpolation expression starts here"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 40,
            "length": 1,
            "message": "Possible closing quote reached before the interpolation was complete"
        }
    ],
    "notes": [],
    "help": [
        "Add '}' to close the interpolation before the string's closing quote."
    ]
}
¬
main :: fn () { prn($"{42}
¬
{
    "message": "Unterminated string literal",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 21,
            "length": 6,
            "message": "String literal starts here"
        }
    ],
    "notes": [],
    "help": [
        "Add a closing double quote to terminate the string literal.",
        "Use `+\"...\"` for an intentional string continuation."
    ]
}
¬
main :: fn () { prn($"{pick("missing
¬
{
    "message": "Unterminated string literal",
    "source_file": "tests/errors/interpolation-delimiters.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 8,
            "message": "String literal starts here"
        }
    ],
    "notes": [],
    "help": [
        "Add a closing double quote to terminate the string literal.",
        "Use `+\"...\"` for an intentional string continuation."
    ]
}