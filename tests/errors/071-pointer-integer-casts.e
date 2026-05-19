main :: fn () {
    ptr: ^u8 = 0
}
¬
{
    "message": "Type mismatch: expected `^u8`, found `untyped integer`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
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
            "message": "This integer cannot be used directly as a pointer"
        }
    ],
    "notes": [],
    "help": [
        "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
    ]
}
¬
main :: fn () {
    ptr: ^u8 = 0.as(usize)
}
¬
{
    "message": "Type mismatch: expected `^u8`, found `usize`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
    "primary_location": {
        "line": 2,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 17,
            "length": 1,
            "message": "This integer cannot be used directly as a pointer"
        }
    ],
    "notes": [],
    "help": [
        "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
    ]
}
¬
take :: fn (ptr: ^u8) {}

main :: fn () {
    take(0)
}
¬
{
    "message": "Type mismatch: expected `^u8`, found `untyped integer`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
    "primary_location": {
        "line": 4,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 10,
            "length": 1,
            "message": "This integer cannot be used directly as a pointer"
        }
    ],
    "notes": [],
    "help": [
        "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
    ]
}
¬
make :: fn () -> ^u8 {
    return 0
}

main :: fn () {}
¬
{
    "message": "Type mismatch: expected `^u8`, found `untyped integer`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 1,
            "message": "This integer cannot be used directly as a pointer"
        }
    ],
    "notes": [],
    "help": [
        "Use `nil` for a null pointer, or use `.as(^T)` when an integer address is intentional."
    ]
}
¬
main :: fn () {
    value: i32 = 4096
    ptr := value.as(^u8)
}
¬
{
    "message": "Cannot cast `i32` to `^u8`",
    "source_file": "tests/errors/071-pointer-integer-casts.e",
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
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
