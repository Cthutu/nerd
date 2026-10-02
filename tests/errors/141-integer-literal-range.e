main :: fn () {
 value: u8 = 256
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: atomic[u8] = 256
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 22,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: i8 = 128
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: i8 = -129
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: u8 = -1
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: u16 = 65536
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u16`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
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
main :: fn () {
 value: i16 = -32769
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i16`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: u32 = 4294967296
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: i32 = 2147483648
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: i64 = 9223372036854775808
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i64`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 19,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: i64 = -9223372036854775809
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i64`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: u64 = -1
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u64`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
N :: 256
main :: fn () {
 value: u8 = N
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 3,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 14,
            "length": 1,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 original := 256
 value: u8 = original
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 14,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
take :: fn (value: u8) { _ := value }
main :: fn () { take(256) }
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 22,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () -> u8 { return 256 }
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 1,
        "column": 30
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 30,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
P :: plex { count u8 }
main :: fn () {
 value: P = {count: 256}
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 3,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 21,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: [2]u8 = [0, 256]
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
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
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value: u8 = 0
 value = 256
 _ := value
}
¬
{
    "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 3,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value := 2147483648
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 11
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 11,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
value := 2147483648
main :: fn () { _ := value }
¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 1,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 10,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value := [2147483648]
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
main :: fn () {
 value := (2147483648, 1)
 _ := value
}
¬
{
    "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
    "source_file": "tests/errors/141-integer-literal-range.e",
    "primary_location": {
        "line": 2,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 12,
            "length": 10,
            "message": "This expression has type `out-of-range integer literal`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
