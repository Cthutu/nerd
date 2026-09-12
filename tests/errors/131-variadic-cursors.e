bad :: fn (args: ...) {}
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `non-generic variadic function with at least one fixed parameter`, found `invalid variadic signature`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 2,
            "message": "This expression has type `invalid variadic signature`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (n: i32, args: ..., more: i32) {}
main :: fn () {}

¬
{
    "message": "Expected RightParen `)` but found Comma `,`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 29
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 29,
            "length": 1,
            "message": "Found Comma `,` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}

¬
bad :: fn (args: i32, args: ...) {}
main :: fn () {}

¬
{
    "message": "Duplicate binding for symbol `args`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 23,
            "length": 4,
            "message": "This binding redefines `args`"
        },
        {
            "kind": "secondary",
            "line": 1,
            "column": 12,
            "length": 4,
            "message": "Previous binding of `args` is here"
        }
    ],
    "notes": [],
    "help": [
        "Rename one of the bindings or remove the duplicate definition."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x := args.next[f32]() }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `promoted C variadic type (i32, u32, i64, u64, isize, usize, f64, or pointer)`, found `f32`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 54
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 54,
            "length": 1,
            "message": "This expression has type `f32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x := args.next[i8]() }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `promoted C variadic type (i32, u32, i64, u64, isize, usize, f64, or pointer)`, found `i8`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 53
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 53,
            "length": 1,
            "message": "This expression has type `i8`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x := args.next[string]() }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `promoted C variadic type (i32, u32, i64, u64, isize, usize, f64, or pointer)`, found `string`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 57
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 57,
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
bad :: fn (_n: i32, args: ...) { _x := args.next() }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `args.next[T](), args.copy(), or args.format(format)`, found `invalid argument cursor operation`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 49
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 49,
            "length": 1,
            "message": "This expression has type `invalid argument cursor operation`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x := args.next[i32](1) }
main :: fn () {}

¬
{
    "message": "Argument count mismatch: expected 0, found 1",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 54
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 54,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 0 arguments to match the function signature."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x := args }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `explicit args.copy()`, found `argument cursor assignment`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 34
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 34,
            "length": 2,
            "message": "This expression has type `argument cursor assignment`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { _x: ^VaList = ^args }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `VaList used directly as a borrowed parameter or local copy`, found `stored or addressed argument cursor`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 16,
            "length": 3,
            "message": "This expression has type `stored or addressed argument cursor`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) -> VaList { return args }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `non-escaping return type`, found `VaList`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 2,
            "message": "This expression has type `VaList`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) => args
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `borrowed cursor or explicit local copy`, found `escaping or assigned VaList`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 2,
            "message": "This expression has type `escaping or assigned VaList`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32, args: ...) { args = args.copy() }
main :: fn () {}

¬
{
    "message": "Cannot assign to `args`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 34
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 34,
            "length": 4,
            "message": "`args` is not a mutable variable"
        }
    ],
    "notes": [],
    "help": [
        "Declare `args` as a variable with `:` or assign to a different mutable symbol."
    ]
}

¬
bad :: fn () { _x: VaList }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `local initialised with args.copy()`, found `argument cursor storage`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 16
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 16,
            "length": 2,
            "message": "This expression has type `argument cursor storage`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Bad :: plex { args VaList }
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `VaList used directly as a borrowed parameter or local copy`, found `stored or addressed argument cursor`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 20
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 20,
            "length": 6,
            "message": "This expression has type `stored or addressed argument cursor`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
pub nrt_va_done :: fn () {}
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `export name outside reserved nrt_ namespace`, found `nrt_va_done`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 5,
            "length": 11,
            "message": "This expression has type `nrt_va_done`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn [T] (_n: T, args: ...) {}
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `non-generic variadic function with required runtime parameters`, found `invalid variadic signature`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 23
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 23,
            "length": 4,
            "message": "This expression has type `invalid variadic signature`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: i32 = 1, args: ...) {}
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `non-generic variadic function with required runtime parameters`, found `invalid variadic signature`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 25
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 25,
            "length": 4,
            "message": "This expression has type `invalid variadic signature`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
bad :: fn (_n: :i32, args: ...) {}
main :: fn () {}

¬
{
    "message": "Type mismatch: expected `non-generic variadic function with required runtime parameters`, found `invalid variadic signature`",
    "source_file": "tests/errors/131-variadic-cursors.e",
    "primary_location": {
        "line": 1,
        "column": 22
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 22,
            "length": 4,
            "message": "This expression has type `invalid variadic signature`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
