bad_param :: ffi "c" bad (value: string)

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `FFI-safe parameter type`, found `string`",
  "source_file": "tests/errors/ffi.e",
  "primary_location": {
    "line": 1,
    "column": 34
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 34,
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
  "source_file": "tests/errors/ffi.e",
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
ffi "c" fcntl (fd: i32, command: i32, ...) -> i32

main :: fn() {
    fcntl(0, 1, "bad")
}
¬
{
  "message": "Type mismatch: expected `FFI-safe vararg type`, found `string`",
  "source_file": "tests/errors/ffi.e",
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
  "message": "Expected declaration or expression but found Ellipsis `...`",
  "source_file": "tests/errors/ffi.e",
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
    "Function parameters require names; write `name: Type`."
  ]
}

¬
lib :: 1
bad_lib_type :: ffi lib bad () -> i32

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `string`, found `untyped integer`",
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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

¬
Point :: plex {
    x i32
}

bad_point_local :: ffi "c" bad_point (point: Point)

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `FFI-safe parameter type`, found `Point`",
  "source_file": "tests/errors/ffi.e",
  "primary_location": {
    "line": 5,
    "column": 46
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 46,
      "length": 5,
      "message": "This expression has type `Point`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/ffi.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Mode :: enum {
    A
    B
}

bad_mode_local :: ffi "c" bad_mode () -> Mode

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `FFI-safe return type`, found `Mode`",
  "source_file": "tests/errors/ffi.e",
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
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/ffi.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Mode` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
bad_tuple_local :: ffi "c" bad_tuple (value: (i32, i32))

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `FFI-safe parameter type`, found `(i32, i32)`",
  "source_file": "tests/errors/ffi.e",
  "primary_location": {
    "line": 1,
    "column": 46
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 46,
      "length": 1,
      "message": "This expression has type `(i32, i32)`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
ffi "c" {
    stdin :: 0.as(^void)
}

main :: fn () {}
¬
{
  "message": "Expected LeftParen `(` but found Integer",
  "source_file": "tests/errors/ffi.e",
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
      "message": "Found Integer here"
    }
  ],
  "notes": [
    "FFI blocks only contain foreign function signatures of the form `name (...) -> T` or `local :: foreign (...) -> T`."
  ],
  "help": [
    "Move constants and Nerd bindings outside the `ffi` block."
  ]
}

¬
ffi "c" {
    bad () => i32
}

main :: fn () {}
¬
{
  "message": "Expected ThinArrow `->` but found FatArrow `=>`",
  "source_file": "tests/errors/ffi.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 2,
      "message": "Found FatArrow `=>` here"
    }
  ],
  "notes": [
    "FFI signatures use `->` for return types; `=>` starts an expression function body."
  ],
  "help": [
    "Write `name (...) -> T` inside an `ffi` block."
  ]
}

¬
ffi "c" strlen (^i8) -> usize
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/ffi.e",
  "primary_location": {
    "line": 1,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 17,
      "length": 1,
      "message": "Found Caret `^` here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
bad :: fn (args: ...) {}
main :: fn () {}

¬
{
  "message": "Type mismatch: expected `non-generic variadic function with at least one fixed parameter`, found `invalid variadic signature`",
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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
  "source_file": "tests/errors/ffi.e",
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


¬
Callback :: fn (context: ^void) -> u32
ffi "callback-test" { invoke (callback: Callback) }
wrong :: fn (context: ^void) -> u64 { return 0 }
main :: fn () { invoke(wrong) }
¬
{
    "message": "Type mismatch: expected `fn (^void) -> u32`, found `fn (^void) -> u64`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 4,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 24,
            "length": 5,
            "message": "This expression has type `fn (^void) -> u64`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Callback :: fn (context: ^void) -> u32
ffi "callback-test" { invoke (callback: Callback) }
wrong :: fn (context: i64) -> u32 { return 0 }
main :: fn () { invoke(wrong) }
¬
{
    "message": "Type mismatch: expected `fn (^void) -> u32`, found `fn (i64) -> u32`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 4,
        "column": 24
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 24,
            "length": 5,
            "message": "This expression has type `fn (i64) -> u32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Callback :: fn (value: string)
ffi "callback-test" { invoke (callback: Callback) }
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `fn (string) -> void`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 2,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 41,
            "length": 8,
            "message": "This expression has type `fn (string) -> void`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Callback :: fn () -> []i32
ffi "callback-test" { invoke (callback: Callback) }
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `fn () -> []i32`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 2,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 41,
            "length": 8,
            "message": "This expression has type `fn () -> []i32`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Callback :: fn (next: fn (value: string))
ffi "callback-test" { invoke (callback: Callback) }
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `fn (fn (string) -> void) -> void`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 2,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 41,
            "length": 8,
            "message": "This expression has type `fn (fn (string) -> void) -> void`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}

¬
Payload :: plex #c { value i32 }
Callback :: fn (value: Payload)
ffi "callback-test" { invoke (callback: Callback) }
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `FFI-safe parameter type`, found `fn (Payload) -> void`",
    "source_file": "tests/errors/ffi.e",
    "primary_location": {
        "line": 3,
        "column": 41
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 41,
            "length": 8,
            "message": "This expression has type `fn (Payload) -> void`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/ffi.e",
            "line": 1,
            "column": 1,
            "length": 7,
            "message": "Type `Payload` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
