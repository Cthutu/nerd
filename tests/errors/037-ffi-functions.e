bad :: ffi "c" (string)

main :: fn() {}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `string`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 1,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 17,
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
bad :: ffi "c" () -> []u8

main :: fn() {}
¬
{
    "code": "0304",
    "message": "Type mismatch: expected `FFI-safe return type`, found `[]u8`",
    "source_file": "tests/errors/037-ffi-functions.e",
    "primary_location": {
        "line": 1,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 22,
            "length": 1,
            "message": "This expression has type `[]u8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
