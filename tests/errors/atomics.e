main :: fn () {
    value : atomic[string] = "no"
}
¬
{
    "message": "Type mismatch: expected `atomic bool, integer, or thin pointer element`, found `string`",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 2,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 19,
            "length": 1,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
consume :: fn (value: atomic[i32]) {
}

main :: fn () {
}
¬
{
    "message": "Type mismatch: expected `pointer to atomic storage (`^atomic[T]`)`, found `atomic[i32]`",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 1,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 16,
            "length": 5,
            "message": "This expression has type `atomic[i32]`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
use std.atomics

main :: fn () {
    value : atomic[i32] = 0
    result := value.compare_exchange(0, 1, success_order = Relaxed, failure_order = Acquire)
    on result {
        Exchanged => {
        }
        else => {
        }
    }
}
¬
{
    "message": "Type mismatch: expected `failure ordering no stronger than success ordering`, found `invalid compare-exchange ordering pair`",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 5,
        "column": 37
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 37,
            "length": 1,
            "message": "This expression has type `invalid compare-exchange ordering pair`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
use std.atomics

main :: fn () {
    value := 1
    pointer : atomic[^i32] = ^value
    read := pointer^
}
¬
{
    "message": "Cannot dereference an atomic pointer",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 6,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 20,
            "length": 1,
            "message": "`atomic[^T]` stores a pointer atomically; it is not itself an ordinary pointer"
        }
    ],
    "notes": [
        "Atomicity of the pointer does not protect the pointee's lifetime or fields."
    ],
    "help": [
        "Load the value into an ordinary pointer first, for example `pointer := atomic_pointer.load()`."
    ]
}

¬
use std.atomics

main :: fn () {
    value := 1
    pointer : atomic[^i32] = ^value
    advanced := pointer + 1
}
¬
{
    "message": "Type mismatch: expected `^i32`, found `untyped integer`",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 6,
        "column": 27
    },
    "references": [
        {
            "kind": "primary",
            "line": 6,
            "column": 27,
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
bad :: ffi "libc.so.6" bad (value: atomic[i32]) -> atomic[i32]

main :: fn () {
}
¬
{
    "message": "Atomic storage cannot be used as an FFI return type",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 1,
        "column": 58
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 58,
            "length": 1,
            "message": "`atomic[T]` has no promised C ABI representation"
        }
    ],
    "notes": [],
    "help": [
        "Pass an ordinary `T` value, or pass explicitly designed opaque storage through a pointer."
    ]
}

¬
main :: fn () {
    value: atomic[u8] = 0x1_0000_0000_0000_0000
    _ := value
}
¬
{
    "message": "Integer literal is too large",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 2,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 25,
            "length": 23,
            "message": "Literal overflow starts here"
        }
    ],
    "notes": [],
    "help": [
        "Use a smaller integer literal."
    ]
}

¬
main :: fn () {
    original: i32 = 7
    value: atomic[usize] = original
    _ := value
}
¬
{
    "message": "Type mismatch: expected `usize`, found `i32`",
    "source_file": "tests/errors/atomics.e",
    "primary_location": {
        "line": 3,
        "column": 28
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 28,
            "length": 8,
            "message": "This expression has type `i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
