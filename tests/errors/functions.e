add :: fn (a: i32, b: i32) => a + b
main :: fn () => add(20)
¬
{
  "message": "Argument count mismatch: expected 2, found 1",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 21,
      "length": 1,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `b`."
  ]
}

¬
neg :: (0 - 1)
use_u32 :: fn (value: u32) => 0
main :: fn () => use_u32(neg)
¬
{
  "message": "Cannot infer negative integer as `u32`",
  "source_file": "tests/errors/functions.e",
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
      "message": "This value is negative but `u32` is unsigned"
    }
  ],
  "notes": [],
  "help": [
    "Use a non-negative value here, or change the destination type to a signed integer."
  ]
}

¬
use std.io

greet :: fn (name: string) -> string {
    prn($"Hello, {name}!")
}

main :: fn () {
    greet("World")
}
¬
{
  "message": "Missing return for function returning `string`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 3,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 10,
      "length": 2,
      "message": "This function can reach the end without returning a `string` value"
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

¬
{
  "message": "Missing entry point `main`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 0,
      "message": "No `main` function is defined"
    }
  ],
  "notes": [
    "Programs currently require a `main` entry point."
  ],
  "help": [
    "Add `main :: fn () => 0`, `main :: fn (args: []string) => 0`, or another supported function bound to main returning an integer, `?void`, or no type at all."
  ]
}

¬
main :: 1
¬
{
  "message": "Invalid type for entry point `main`: found `untyped integer`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "`main` must be a function with no parameters or one `[]string` parameter, returning an integer, `?void`, or no value"
    }
  ],
  "notes": [],
  "help": [
    "Change `main` to `fn ()`, or to `fn (args: []string)` if the program needs command-line arguments."
  ]
}

¬
add :: fn (a: i32, b: i32) -> i32 => a + b
main :: fn () => add(20, 22)
¬
{
  "message": "Cannot combine an explicit return type with a fat-arrow body",
  "source_file": "tests/errors/functions.e",
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
      "message": "This function uses `->` for an explicit return type and `=>` for an expression body"
    }
  ],
  "notes": [
    "Fat-arrow functions infer their return type from the body expression."
  ],
  "help": [
    "Use `fn (...) => expr` for inferred returns, or rewrite the function as `fn (...) -> T { return expr }`."
  ]
}

¬
bad : fn() -> i32 {
    return 1
}
¬
{
  "message": "Expected Equal `=` but found LeftBrace `{`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 19,
      "length": 1,
      "message": "Found LeftBrace `{` here"
    }
  ],
  "notes": [
    "A function type annotation cannot include a function body."
  ],
  "help": [
    "Function definitions use `::`; did you mean to write `::` instead of `:`?"
  ]
}

¬
bad : fn() -> i32 => 1
¬
{
  "message": "Expected Equal `=` but found FatArrow `=>`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 19,
      "length": 2,
      "message": "Found FatArrow `=>` here"
    }
  ],
  "notes": [
    "A function type annotation cannot include a function body."
  ],
  "help": [
    "Function definitions use `::`; did you mean to write `::` instead of `:`?"
  ]
}

¬
bad : fn(frame: ^Frame) {
}
¬
{
  "message": "Expected Colon `:` but found Keyword `fn`",
  "source_file": "tests/errors/functions.e",
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
      "message": "Found Keyword `fn` here"
    }
  ],
  "notes": [
    "Function values use a binding operator before `fn`, not a type annotation colon."
  ],
  "help": [
    "Use `:: fn (...) { ... }` for a constant function binding, or `:= fn (...) { ... }` for a mutable function value."
  ]
}

¬
bad :: fn (a: i32 = 1, b: i32) => a + b
main :: fn () => bad(1, 2)
¬
{
  "message": "Required parameter `b` cannot follow a defaulted parameter",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 24
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 24,
      "length": 1,
      "message": "This parameter has no default value"
    }
  ],
  "notes": [
    "Parameters with defaults must be the trailing parameters in the signature."
  ],
  "help": [
    "Move `b` before the defaulted parameters or give it a default value."
  ]
}

¬
bad :: fn (a: i32 = b, b: i32 = 2) => a + b
main :: fn () => bad()
¬
{
  "message": "Default parameter cannot reference later parameter `b`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 21,
      "length": 1,
      "message": "`b` is not available while this default is evaluated"
    }
  ],
  "notes": [
    "Default arguments are evaluated at the call site from left to right."
  ],
  "help": [
    "Only reference parameters that appear earlier in the signature."
  ]
}

¬
bad :: fn (a: i32 = "no") => a
main :: fn () => bad()
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 21,
      "length": 4,
      "message": "This expression has type `string`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
puts :: ffi "c" puts (value: string = "x") -> i32
main :: fn () => 0
¬
{
  "message": "FFI parameter `value` cannot have a default value",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 23,
      "length": 5,
      "message": "This default value belongs to an FFI declaration"
    }
  ],
  "notes": [
    "FFI declarations describe the external function's exact C signature."
  ],
  "help": [
    "Remove the default value or wrap the FFI function in a normal Nerd function with defaults."
  ]
}

¬
add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c
main :: fn () => add(a = 1, c = 3)
¬
{
  "message": "Named argument `c` does not match parameter `b`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 29
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 29,
      "length": 1,
      "message": "This argument is named `c`"
    }
  ],
  "notes": [
    "Named arguments are currently checked in parameter order."
  ],
  "help": [
    "Move `c = ...` to the matching parameter position or provide `b = ...` here."
  ]
}

¬
main :: () {
    return
}
¬
{
  "message": "Expected Keyword `fn` but found LeftParen `(`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 9,
      "length": 1,
      "message": "Found LeftParen `(` here"
    }
  ],
  "notes": [
    "Function values after `::` start with `fn` before the parameter list."
  ],
  "help": [
    "Did you forget `fn` before the parameter list? Write `:: fn (...) { ... }`."
  ]
}

¬
main :: fn () {
    local := () {
        return
    }
}
¬
{
  "message": "Expected Keyword `fn` but found LeftParen `(`",
  "source_file": "tests/errors/functions.e",
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
      "message": "Found LeftParen `(` here"
    }
  ],
  "notes": [
    "Function values after `:=` start with `fn` before the parameter list."
  ],
  "help": [
    "Did you forget `fn` before the parameter list? Write `:= fn (...) { ... }`."
  ]
}

¬
main :: f () {
    return
}
¬
{
  "message": "Expected Keyword `fn` but found Symbol",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 9,
      "length": 1,
      "message": "Found Symbol here"
    }
  ],
  "notes": [
    "Function values after `::` start with `fn` before the parameter list."
  ],
  "help": [
    "Did you forget `fn` before the parameter list? Write `:: fn (...) { ... }`."
  ]
}

¬
make_value :: fn () {
    return 1
}

main :: fn () {
    make_value()
}
¬
{
  "message": "Cannot return a value from a function with no return type",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 6,
      "message": "This `return` provides a value, but the function returns no value"
    }
  ],
  "notes": [
    "Functions written without `-> Type` return no value."
  ],
  "help": [
    "Remove the returned expression, or add `-> Type` to the function signature."
  ]
}

¬
normalise :: fn (seed: u64) -> u64 {
    on seed == 0 => seed = 7
    return seed
}

main :: fn () => normalise(0).as(i32)
¬
{
  "message": "Cannot assign to `seed`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 21,
      "length": 4,
      "message": "`seed` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `seed` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
State :: plex {
    player_loc i32
}

move :: fn (state: State) {
    state.player_loc = 1
}

main :: fn () {
    state := State { player_loc: 0 }
    move(state)
}
¬
{
  "message": "Cannot assign to `state`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 6,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 5,
      "length": 5,
      "message": "`state` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `state` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
write :: fn (values: [1]i32) {
    values[0] = 1
}

main :: fn () {
    values: [1]i32 = [0]
    write(values)
}
¬
{
  "message": "Cannot assign to `values`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 6,
      "message": "`values` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `values` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
normalise :: fn (seed: u64) -> u64 {
    (seed) = (7)
    return seed
}

main :: fn () => normalise(0).as(i32)
¬
{
  "message": "Cannot assign to `seed`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 6
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 6,
      "length": 4,
      "message": "`seed` is not a mutable variable"
    }
  ],
  "notes": [],
  "help": [
    "Declare `seed` as a variable with `:` or assign to a different mutable symbol."
  ]
}

¬
empty :: fn {}
main :: fn () {}
¬
{
  "message": "Type mismatch: expected `compound function with at least one member`, found `empty compound function`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 10,
      "length": 2,
      "message": "This expression has type `empty compound function`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Thing :: plex {}

impl Thing {
    pub poll :: fn (self: ^Self, frame: i32, flags: bool) {}
}

main :: fn () {
    thing: Thing = {}
    thing.poll()
}
¬
{
  "message": "Argument count mismatch: expected 2, found 0",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 9,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 15,
      "length": 1,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `frame`.",
    "Pass an argument for parameter `flags`."
  ]
}

¬
bad :: fn (i32) {}
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
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
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Thing :: plex { value i32 }
impl Thing { bad :: fn (^Self) {} }
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
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
Render :: trait { render :: fn (^Self) }
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
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
      "message": "Found Caret `^` here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Render :: trait { render :: fn (Self) }
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
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
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Render :: trait { render :: fn (self: Self, i32) }
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 45
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 45,
      "length": 3,
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
main :: fn () { _callback := fn (i32) {} }
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 34
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 34,
      "length": 3,
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Functions :: plex #c { log fn (^u8, ^u8) }
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 32
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 32,
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
Callback :: fn (i32) -> i32
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 17,
      "length": 3,
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Factory :: fn (value: i32) -> fn (^u8)
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 35
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 35,
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
apply :: fn (callback: fn (i32) -> i32) {}
¬
{
  "message": "Expected declaration or expression but found Symbol",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 28
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 28,
      "length": 3,
      "message": "Found Symbol here"
    }
  ],
  "notes": [],
  "help": [
    "Function parameters require names; write `name: Type`."
  ]
}

¬
Functions :: plex { log fn (channel: ^u8, ^u8) }
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 43
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 43,
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
ffi "c" puts (^i8) -> i32
¬
{
  "message": "Expected declaration or expression but found Caret `^`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 1,
    "column": 15
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 15,
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
add :: fn (a: i32, b: i32 = 2) => a + b
main :: fn () => add(1, 3)
¬
{
  "message": "Defaulted parameter `b` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 1,
      "message": "This argument must be named `b`"
    }
  ],
  "notes": [],
  "help": [
    "Write `b = ...` or omit the argument to use its default."
  ]
}

¬
add :: fn (a: i32 = 2) => a
main :: fn () { alias := add
_ := alias(3) }
¬
{
  "message": "Defaulted parameter `a` requires a named argument",
  "source_file": "tests/errors/functions.e",
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
      "message": "This argument must be named `a`"
    }
  ],
  "notes": [],
  "help": [
    "Write `a = ...` or omit the argument to use its default."
  ]
}

¬
identity :: fn [T] (value: T, enabled: bool = yes) => value
main :: fn () => identity(3, no)
¬
{
  "message": "Defaulted parameter `enabled` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 30
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 30,
      "length": 2,
      "message": "This argument must be named `enabled`"
    }
  ],
  "notes": [],
  "help": [
    "Write `enabled = ...` or omit the argument to use its default."
  ]
}

¬
identity :: fn [T] (value: T, enabled: bool = yes) => value
main :: fn () => identity[i32](3, no)
¬
{
  "message": "Defaulted parameter `enabled` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 35
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 35,
      "length": 2,
      "message": "This argument must be named `enabled`"
    }
  ],
  "notes": [],
  "help": [
    "Write `enabled = ...` or omit the argument to use its default."
  ]
}

¬
choose :: fn (enabled :: bool = yes) => on enabled => 1 else 0
main :: fn () => choose(no)
¬
{
  "message": "Defaulted parameter `enabled` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 2,
      "message": "This argument must be named `enabled`"
    }
  ],
  "notes": [],
  "help": [
    "Write `enabled = ...` or omit the argument to use its default."
  ]
}

¬
Counter :: plex { value i32 }
impl Counter { add :: fn (self: Self, amount: i32 = 1) => self.value + amount }
main :: fn () { counter := Counter { value: 1 }
_ := counter.add(3) }
¬
{
  "message": "Defaulted parameter `amount` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 4,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 18,
      "length": 1,
      "message": "This argument must be named `amount`"
    }
  ],
  "notes": [],
  "help": [
    "Write `amount = ...` or omit the argument to use its default."
  ]
}

¬
use test.named_defaults
main :: fn () => imported(1, 3)
¬
{
  "message": "Defaulted parameter `increment` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 30
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 30,
      "length": 1,
      "message": "This argument must be named `increment`"
    }
  ],
  "notes": [],
  "help": [
    "Write `increment = ...` or omit the argument to use its default."
  ]
}

¬
use test.named_defaults
main :: fn () => imported_generic(1, no)
¬
{
  "message": "Defaulted parameter `enabled` requires a named argument",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 2,
      "message": "This argument must be named `enabled`"
    }
  ],
  "notes": [],
  "help": [
    "Write `enabled = ...` or omit the argument to use its default."
  ]
}

¬
add :: fn (a: i32, b: i32 = 2) => a + b
main :: fn () => add(1, wrong = 3)
¬
{
  "message": "Named argument `wrong` does not match parameter `b`",
  "source_file": "tests/errors/functions.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 5,
      "message": "This argument is named `wrong`"
    }
  ],
  "notes": [
    "Named arguments are currently checked in parameter order."
  ],
  "help": [
    "Move `wrong = ...` to the matching parameter position or provide `b = ...` here."
  ]
}


¬
main :: fn () {
    x := 1
    f := fn () => x
    y := 0
    return y
}
¬
{
  "message": "Cannot capture outer binding `x`",
  "source_file": "tests/errors/functions.e",
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
      "message": "Nested functions are non-closures, so `x` is not available here"
    }
  ],
  "notes": [],
  "help": [
    "Pass `x` as a parameter instead of referencing it from the enclosing function scope."
  ]
}
