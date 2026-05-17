bad_param :: ffi "c" bad (string)

main :: fn() {}
¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `string`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 1,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 27,
            "length": 6,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
bad_return :: ffi "c" bad () -> []u8

main :: fn() {}
¬
{
    "message": "Type mismatch: expected `FFI-safe return type`, found `[]u8`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 1,
        "column": 33
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 33,
            "length": 1,
            "message": "This expression has type `[]u8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
ffi "c" fcntl (i32, i32, ...) -> i32

main :: fn() {
    fcntl(0, 1, "bad")
}
¬
{
    "message": "Type mismatch: expected `FFI-safe vararg type`, found `string`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 4,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 17,
            "length": 5,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
bad :: fn (...) -> i32

main :: fn() {}
¬
{
    "message": "Expected Symbol but found Ellipsis `...`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 1,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 12,
            "length": 3,
            "message": "Found Ellipsis `...` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
¬
lib :: 1
bad_lib_type :: ffi lib bad () -> i32

main :: fn() {}
¬
{
    "message": "Type mismatch: expected `string`, found `untyped integer`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 3,
            "message": "This expression has type `untyped integer`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
lib : string
bad_lib_runtime :: ffi lib bad () -> i32

main :: fn() {}
¬
{
    "message": "Type mismatch: expected `compile-time string`, found `string`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 15,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
