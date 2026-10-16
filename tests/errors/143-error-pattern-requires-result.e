Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.Bad! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 27,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.WithCode { code }! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 41,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { error! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 21,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { _! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: ?Failure) {
    on value { Failure.Bad! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `?Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 27,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `?Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: i32) {
    on value { 1! => {} else => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `i32`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 17,
            "length": 1,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
    ]
}
¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.Bad => {} } else { Failure.WithCode { code } => {} }
}
main :: fn () {}
¬
{
    "message": "Error-channel pattern requires a result value, found `Failure`",
    "source_file": "tests/errors/143-error-pattern-requires-result.e",
    "primary_location": {
        "line": 3,
        "column": 43
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 43,
            "length": 7,
            "message": "This pattern selects an error channel"
        },
        {
            "kind": "secondary",
            "line": 3,
            "column": 8,
            "length": 5,
            "message": "This `on` subject has type `Failure`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/143-error-pattern-requires-result.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Failure` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Match this value in the first pattern table; an error pattern table requires a result subject of type `T\\U`."
    ]
}
