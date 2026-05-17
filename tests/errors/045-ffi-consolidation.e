Point :: plex {
    x i32
}

bad_point_local :: ffi "c" bad_point (Point)

main :: fn() {}¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `Point`",
    "source_file": "tests/errors/045-ffi-consolidation.e",
    "primary_location": {
        "line": 5,
        "column": 39
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 39,
            "length": 5,
            "message": "This expression has type `Point`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬Mode :: enum {
    A
    B
}

bad_mode_local :: ffi "c" bad_mode () -> Mode

main :: fn() {}¬
{
    "message": "Type mismatch: expected `FFI-safe return type`, found `Mode`",
    "source_file": "tests/errors/045-ffi-consolidation.e",
    "primary_location": {
        "line": 6,
        "column": 42
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 42,
            "length": 4,
            "message": "This expression has type `Mode`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬bad_tuple_local :: ffi "c" bad_tuple ((i32, i32))

main :: fn() {}¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `(i32, i32)`",
    "source_file": "tests/errors/045-ffi-consolidation.e",
    "primary_location": {
        "line": 1,
        "column": 39
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 39,
            "length": 1,
            "message": "This expression has type `(i32, i32)`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
