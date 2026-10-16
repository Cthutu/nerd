Colour :: enum { Red Green = 4 Blue }
main :: fn () -> i32 { return Colour.Red }

¬
{
    "message": "Type mismatch: expected `i32`, found `Colour`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 3,
            "message": "This expression has type `Colour`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/150-enum-integer-contexts.e",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Type `Colour` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Red Green = 4 Blue }
main :: fn () -> i32 { return Colour.Blue }

¬
{
    "message": "Type mismatch: expected `i32`, found `Colour`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 4,
            "message": "This expression has type `Colour`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/150-enum-integer-contexts.e",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Type `Colour` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 4 }
main :: fn () -> i32 { colour := Colour.Green return colour }

¬
{
    "message": "Type mismatch: expected `i32`, found `Colour`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 54
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 54,
            "length": 6,
            "message": "This expression has type `Colour`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/150-enum-integer-contexts.e",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Type `Colour` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 4 }
green: Colour: Colour.Green
main :: fn () -> i32 { return green }

¬
{
    "message": "Type mismatch: expected `i32`, found `Colour`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 3,
        "column": 31
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 31,
            "length": 5,
            "message": "This expression has type `Colour`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/150-enum-integer-contexts.e",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "Type `Colour` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Value :: enum { None Some(i32) = 4 }
main :: fn () -> i32 { return Value.Some(42) }

¬
{
    "message": "Type mismatch: expected `i32`, found `Value`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 41,
            "length": 1,
            "message": "This expression has type `Value`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/150-enum-integer-contexts.e",
            "line": 1,
            "column": 1,
            "length": 5,
            "message": "Type `Value` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 256 }
main :: fn () { _number: u8 = Colour.Green }

¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 5,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 128 }
main :: fn () { _number: i8 = Colour.Green }

¬
{
    "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 5,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 4294967295 }
main :: fn () -> i32 { return Colour.Green }

¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 38
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 38,
            "length": 5,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
Colour :: enum { Green = 256 }
green :: Colour.Green
main :: fn () { _number: u8 = green }

¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 3,
        "column": 31
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 31,
            "length": 5,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
other :: use enum_integer_contexts
main :: fn () -> i32 { return other.Colour.Blue }

¬
{
    "message": "Type mismatch: expected `i32`, found `Colour`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 2,
        "column": 44
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 44,
            "length": 4,
            "message": "This expression has type `Colour`"
        },
        {
            "kind": "secondary",
            "source_file": "__REPO__/tests/mods/enum_integer_contexts.n",
            "line": 1,
            "column": 5,
            "length": 6,
            "message": "Type `Colour` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Colour :: enum { Green = 4 }
wide :: fn (value: i32) => value
small :: fn (value: u8) => value
choose :: fn { wide small }
main :: fn () { _value := choose(Colour.Green) }

¬
{
    "message": "Type mismatch: expected `exactly one compatible compound function member`, found `ambiguous compound call`",
    "source_file": "tests/errors/150-enum-integer-contexts.e",
    "primary_location": {
        "line": 5,
        "column": 33
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 33,
            "length": 1,
            "message": "This expression has type `ambiguous compound call`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
