main :: fn () {
 a: u32 = 42
 b := a.as()
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `concrete contextual cast destination`, found `undetermined type`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 8,
            "length": 1,
            "message": "This expression has type `undetermined type`"
        }
    ],
    "notes": [],
    "help": [
        "Add a destination type annotation or write `.as(Type)` explicitly."
    ]
}
¬
main :: fn () {
 a: u32 = 42
 b := a.as() + 1
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `concrete contextual cast destination`, found `undetermined type`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 8,
            "length": 1,
            "message": "This expression has type `undetermined type`"
        }
    ],
    "notes": [],
    "help": [
        "Add a destination type annotation or write `.as(Type)` explicitly."
    ]
}
¬
main :: fn () {
 a := "hello"
 b: f64 = a.as()
 prn($"{b}")
}
¬
{
    "message": "Cannot cast `string` to `f64`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 1,
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
¬
main :: fn () {
 a := [1, 2]
 b: []u8 = a.as()
 prn($"{b}")
}
¬
{
    "message": "Cannot cast `[2]i32` to `[]u8`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 13,
            "length": 1,
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
¬
main :: fn () {
 a := [1, 2]
 b: []i32 = a.data.as()
 prn($"{b}")
}
¬
{
    "message": "Cannot cast `^i32` to `[]i32`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 19,
            "length": 1,
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
¬
main :: fn () {
 a := [1, 2]
 b: []i32 = a.data.as("two")
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `usize`, found `string`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 23,
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
main :: fn () {
 a := [1, 2]
 n: usize = 2
 b := a.data.as(n)
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `concrete contextual cast destination`, found `undetermined type`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 4,
        "column": 13
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 13,
            "length": 1,
            "message": "This expression has type `undetermined type`"
        }
    ],
    "notes": [],
    "help": [
        "Add a destination type annotation or write `.as(Type)` explicitly."
    ]
}
¬
main :: fn () {
 a: u32 = 42
 b: u64 = a.as() + a.as(i32)
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `i32`, found `u64`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 1,
            "message": "This expression has type `u64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
id :: fn [T] (value: T) -> T { return value }
main :: fn () {
 a: u32 = 42
 b := id(a.as())
 prn($"{b}")
}
¬
{
    "message": "Type mismatch: expected `inferable generic type parameter`, found `T`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 1,
        "column": 7
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 7,
            "length": 2,
            "message": "This expression has type `T`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
take_u64 :: fn (_value: u64) {}
take_u32 :: fn (_value: u32) {}
main :: fn () {
    source: i32 = 7
    cast := source.as()
    alias := cast
    take_u64(cast)
    take_u32(alias)
}
¬
{
    "message": "Type mismatch: expected `u64`, found `u32`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 5,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 19,
            "length": 1,
            "message": "This expression has type `u32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
¬
take_u64 :: fn (_value: u64) {}
main :: fn () {
    source := "hello"
    cast := source.as()
    take_u64(cast)
}
¬
{
    "message": "Cannot cast `string` to `u64`",
    "source_file": "tests/errors/contextual-casts.e",
    "primary_location": {
        "line": 4,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 19,
            "length": 1,
            "message": "This cast is not supported"
        }
    ],
    "notes": [],
    "help": [
        "Use explicit casts only between compatible primitive types."
    ]
}
