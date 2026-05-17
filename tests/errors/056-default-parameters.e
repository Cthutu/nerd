bad :: fn (a: i32 = 1, b: i32) => a + b
main :: fn () => bad(1, 2)
¬
{
    "message": "Required parameter `b` cannot follow a defaulted parameter",
    "source_file": "tests/errors/056-default-parameters.e",
    "primary_location": {
        "line": 1,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 24,
            "length": 1,
            "message": "This parameter has no default value"
        }
    ],
    "notes": [
        "Parameters with defaults must be the trailing parameters in the signature."
    ],
    "help": [
        "Move `b` before the defaulted parameters or give it a default value."
    ]
}
¬
bad :: fn (a: i32 = b, b: i32 = 2) => a + b
main :: fn () => bad()
¬
{
    "message": "Default parameter cannot reference later parameter `b`",
    "source_file": "tests/errors/056-default-parameters.e",
    "primary_location": {
        "line": 1,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 21,
            "length": 1,
            "message": "`b` is not available while this default is evaluated"
        }
    ],
    "notes": [
        "Default arguments are evaluated at the call site from left to right."
    ],
    "help": [
        "Only reference parameters that appear earlier in the signature."
    ]
}
¬
bad :: fn (a: i32 = "no") => a
main :: fn () => bad()
¬
{
    "message": "Type mismatch: expected `i32`, found `string`",
    "source_file": "tests/errors/056-default-parameters.e",
    "primary_location": {
        "line": 1,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 21,
            "length": 4,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
puts :: ffi "c" puts (value: string = "x") -> i32
main :: fn () => 0
¬
{
    "message": "FFI parameter `value` cannot have a default value",
    "source_file": "tests/errors/056-default-parameters.e",
    "primary_location": {
        "line": 1,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 23,
            "length": 5,
            "message": "This default value belongs to an FFI declaration"
        }
    ],
    "notes": [
        "FFI declarations describe the external function's exact C signature."
    ],
    "help": [
        "Remove the default value or wrap the FFI function in a normal Nerd function with defaults."
    ]
}
¬
