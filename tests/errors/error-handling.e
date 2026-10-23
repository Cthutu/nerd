main :: fn () {
    defer break
}
¬
{
  "message": "`break` can only be used inside a loop or expression block",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 11,
      "length": 5,
      "message": "This `break` is not inside a `for` loop or expression block"
    }
  ],
  "notes": [],
  "help": [
    "Move `break` into a `for` loop or expression block."
  ]
}

¬
main :: fn () {
    defer again
}
¬
{
  "message": "`again` can only be used inside a loop",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 11
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 11,
      "length": 5,
      "message": "This `again` is not inside a `for` loop"
    }
  ],
  "notes": [],
  "help": [
    "Move `again` into a `for` loop body."
  ]
}

¬
bad :: fn () -> ! {
    return 1
}

main :: fn () {}
¬
{
  "message": "Type mismatch: expected `!`, found `untyped integer`",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
bad :: fn () -> ! {
}

main :: fn () {}
¬
{
  "message": "Missing return for function returning `!`",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "This function can reach the end without returning a `!` value"
    }
  ],
  "notes": [
    "Block-bodied functions with explicit return types must return a value before they end."
  ],
  "help": [
    "Add `return <expr>` to the function body or remove the explicit return type if the function should not produce a value."
  ]
}

¬
bad :: fn (value: i32) -> i32 {
    return value?
}

main :: fn () {}
¬
{
  "message": "Type mismatch: expected `optional or result value`, found `i32`",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "This expression has type `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
bad :: fn () -> i32 {
    return "failed"!
}

main :: fn () {}
¬
{
  "message": "Type mismatch: expected `contextual result type T\\E`, found `error injection without an expected result type`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 20,
      "length": 1,
      "message": "This expression has type `error injection without an expected result type`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Failure :: enum {
    Bad
}

value :: fn () -> i32\Failure {
    return 1
}

main :: fn () {
    result := value()?
}
¬
{
  "message": "Cannot propagate `Failure` from a function returning `void`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 10,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 10,
      "column": 22,
      "length": 1,
      "message": "This `?` can return `Failure` to the caller"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [
    "The enclosing function has no compatible failure channel"
  ],
  "help": [
    "Handle the failure here, or change the function return type to `void\\Failure`"
  ]
}

¬
PixelLayer :: plex {
}

GfxError :: enum {
    Missing
}

get_layer :: fn () -> ^PixelLayer\GfxError {
    return Missing!
}

main :: fn () {
    layer := get_layer()
    value := layer[0]
}
¬
{
  "message": "Type mismatch: expected `array, slice, dynamic array, string, or pointer`, found `^PixelLayer\\GfxError`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 14,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 14,
      "column": 19,
      "length": 1,
      "message": "This expression has type `^PixelLayer\\GfxError`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 10,
      "message": "Type `PixelLayer` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 4,
      "column": 1,
      "length": 8,
      "message": "Type `GfxError` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { undo prn("invalid") }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 17,
      "length": 4,
      "message": "The enclosing function must return an optional or result"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
main :: fn () -> i32 { undo prn("invalid")
return 0 }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 24
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 24,
      "length": 4,
      "message": "The enclosing function must return an optional or result"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
main :: fn () -> ?void { undo return }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 31
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 31,
      "length": 6,
      "message": "An undo action cannot return, propagate failure, or register another undo"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
fail :: fn () -> ?void { return nil }
main :: fn () -> ?void { undo fail()? }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 37
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 37,
      "length": 1,
      "message": "An undo action cannot return, propagate failure, or register another undo"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
main :: fn () -> ?void { undo { undo prn("nested") } }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 33
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 33,
      "length": 4,
      "message": "An undo action cannot return, propagate failure, or register another undo"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
main :: fn () -> ?void { defer { undo prn("late") } }
¬
{
  "message": "Invalid `undo` statement",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "An undo action cannot be registered from another cleanup action"
    }
  ],
  "notes": [],
  "help": [
    "Use `undo` inside an optional- or result-returning function, with cleanup that does not return or propagate failure."
  ]
}

¬
main :: fn () -> ?void { for yes { undo break
break } }
¬
{
  "message": "`break` can only be used inside a loop or expression block",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 41
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 41,
      "length": 5,
      "message": "This `break` is not inside a `for` loop or expression block"
    }
  ],
  "notes": [],
  "help": [
    "Move `break` into a `for` loop or expression block."
  ]
}

¬
main :: fn () -> ?void { for yes { undo again
break } }
¬
{
  "message": "`again` can only be used inside a loop",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 41
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 41,
      "length": 5,
      "message": "This `again` is not inside a `for` loop"
    }
  ],
  "notes": [],
  "help": [
    "Move `again` into a `for` loop body."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.Bad! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `Failure`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 27,
      "length": 1,
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.WithCode { code }! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `Failure`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 41
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 41,
      "length": 1,
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { error! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `Failure`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 21,
      "length": 1,
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { _! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `Failure`",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: ?Failure) {
    on value { Failure.Bad! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `?Failure`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 27,
      "length": 1,
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `?Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: i32) {
    on value { 1! => {} else => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `i32`",
  "source_file": "tests/errors/error-handling.e",
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
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `i32`"
    }
  ],
  "notes": [],
  "help": [
    "Remove `!` to match this value directly; error-channel patterns require a result subject of type `T\\U`."
  ]
}

¬
Failure :: enum { Bad WithCode { code i32 } }
name :: fn (value: Failure) {
    on value { Failure.Bad => {} } else { Failure.WithCode { code } => {} }
}
main :: fn () {}
¬
{
  "message": "Error-channel pattern requires a result value, found `Failure`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 43
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 43,
      "length": 7,
      "message": "This pattern selects an error channel"
    },
    {
      "kind": "secondary",
      "line": 3,
      "column": 8,
      "length": 5,
      "message": "This `on` subject has type `Failure`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/error-handling.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Failure` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Match this value in the first pattern table; an error pattern table requires a result subject of type `T\\U`."
  ]
}

¬
main :: fn () { value := 7 \ 42 }
¬
{
  "message": "Type mismatch: expected `optional or result value`, found `untyped integer`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 26,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "The left operand of `\\` must have type `?T` or `T\\E`."
  ]
}

¬
main :: fn () {
    value : ?i32
    recovered := value \ "wrong"
}
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 26,
      "length": 7,
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
    value : ?u8
    recovered := value \ 256
}
¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 26,
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
    value : i32\string
    other : ?i32
    recovered := value \ other
}
¬
{
  "message": "Type mismatch: expected `i32`, found `?i32`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 4,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 26,
      "length": 5,
      "message": "This expression has type `?i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value : ?i32
    recovered := value \ }
¬
{
  "message": "Missing value before RightBrace `}`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 26,
      "length": 1,
      "message": "RightBrace `}` cannot appear here"
    }
  ],
  "notes": [],
  "help": [
    "Insert a literal, parenthesized expression, or unary operator"
  ]
}

¬
main :: fn () {
    value : ?void
    value \ 42
}
¬
{
  "message": "Type mismatch: expected `void`, found `untyped integer`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 13,
      "length": 2,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    value : ?i32
    value \ 42
}
¬
{
  "message": "Expression result of type `i32` is not used",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 5,
      "length": 5,
      "message": "This expression produces a value, but the value is discarded"
    }
  ],
  "notes": [
    "Only `void` expressions can be used as standalone statements."
  ],
  "help": [
    "Bind the result to `_` when the value is intentionally ignored."
  ]
}

¬
main :: fn () { value := on 7 else { return } }
¬
{
  "message": "Expected FatArrow `=>` but found RightBrace `}`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 47
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 47,
      "length": 1,
      "message": "Found RightBrace `}` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := on work() else? {} }
¬
{
  "message": "Expected LeftBrace `{` but found Question `?`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 40,
      "length": 1,
      "message": "Found Question `?` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := work() else? {} }
¬
{
  "message": "Expected LeftBrace `{` but found Question `?`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 37
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 37,
      "length": 1,
      "message": "Found Question `?` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
main :: fn () { value := 7 else {} }
¬
{
  "message": "Type mismatch: expected `optional or result value`, found `untyped integer`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 1,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 26,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "The subject before `else` must have type `?T` or `T\\E`."
  ]
}

¬
main :: fn () {
    value : ?i32
    found := value else [error] {}
}
¬
{
  "message": "Type mismatch: expected `optional else branch without a payload binder`, found `payload binder`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 20,
      "length": 4,
      "message": "This expression has type `payload binder`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
work :: fn () -> i32\string { return 7 }
main :: fn () -> i32 { value := work() else {} return value }
¬
{
  "message": "Cannot propagate `string` from a function returning `i32`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 40,
      "length": 4,
      "message": "This `?` can return `string` to the caller"
    }
  ],
  "notes": [
    "The enclosing function has no compatible failure channel"
  ],
  "help": [
    "Handle the failure here, or change the function return type to `i32\\string`"
  ]
}

¬
work :: fn () -> ?i32 { return 7 }
main :: fn () -> i32 { value := work() else {} return value }
¬
{
  "message": "Cannot propagate `absence` from a function returning `i32`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 40,
      "length": 4,
      "message": "This `?` can return `absence` to the caller"
    }
  ],
  "notes": [
    "The enclosing function has no compatible failure channel"
  ],
  "help": [
    "Handle the failure here, or change the function return type to `?i32`"
  ]
}

¬
work :: fn () -> i32\string { return 7 }
main :: fn () { value := work() else ${ break 42 } }
¬
{
  "message": "Expected LeftBrace `{` but found Dollar `$`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 1,
      "message": "Found Dollar `$` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
work :: fn () -> ?i32 { return 7 }
main :: fn () { value := work() else return }
¬
{
  "message": "Expected LeftBrace `{` but found Keyword `return`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 6,
      "message": "Found Keyword `return` here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
main :: fn () { value: ?i32 = 42
 _flag: bool = value }
¬
{
  "message": "Type mismatch: expected `bool`, found `?i32`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 5,
      "message": "This expression has type `?i32`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
accept :: fn (_flag: bool) {}
main :: fn () { value: ?bool = no
 accept(value) }
¬
{
  "message": "Type mismatch: expected `bool`, found `?bool`",
  "source_file": "tests/errors/error-handling.e",
  "primary_location": {
    "line": 3,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 9,
      "length": 5,
      "message": "This expression has type `?bool`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}
