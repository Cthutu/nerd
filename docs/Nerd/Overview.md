Nerd is a new programming language that has feature parity with C and is intended as a replacement for C.

This means that it needs to support everything C does (such as structs, functions, unions, bitfields etc).

On top of that, Nerd will support:
- Slices, sometimes called fat pointers, where a pointer and size are wrapped into one value.  This also includes the `[.. : ..]` syntax that can create sub-slices.
- Pattern matching
- Destructuring structs and tuples
- Improved literal syntax for structs, union types
- Support for tuples and multiple return types
- Support for tuple structs (e.g. `struct(u32, f64)` as used in language like Rust)
- Automatic pointer dereferencing (no `->` syntax for pointers, only use `.`)
- Proper module system
- Nested functions
- More consistent pointer syntax.  Pointer to type has a `^` prefix the type.  Dereferencing a pointer variable has `^` come after the variable
- FFI declarations for C calls (including variadic syntax only for this purpose)
- Labelled loops.
- Expression blocks.
- Interpolated strings.

In the future and not required initially, Nerd 2.0 will also support:
- Traits
- Generics
- Compile time execution of code via a VM to calculate and bake values while compiling
- `impl` blocks related to traits to associate functions with types
- Algebraic data types (sum types)

# Error Reporting

Nerd also has a very high quality error reporting system.  When an error occurs, one of the many internal error function are called with all the parameters required to report the error.  An error is categorised with a 4-digit error code.  For example, all lexical analysis errors have error codes in the range of `0100`-`0199` (the leading `01` represents the lexical analysis stage).  This means, that if an unknown character is error `0100`, an internal function like `E0100_unknown_character()` is called with the source file information, offset and unknown character.

These error functions (one per error category) will create an error report that contains various error widgets.  An error widget could include:

- The error code along with the generate error message
- A code snippet showing the line of code and previous/next couple of lines, along with pointers and individual messages.
- A note message block
- A help message block

This allows us to generate two types of messages: one for normal use, where the widgets are rendered with word wrap (according to current terminal width), and can even be placed in a Unicode line drawn rectangle that houses the whole error message; and another for use in testing, which has no word wrapping or decorations.  This allows us to run unit tests on error message production.

The error messages are colourful and formatted appropriately.  These are heavily inspired by Rust like messages.

Here's a monochrome example:

```
error[E0100]: Unknown character `😄`
 --> src/foo.n
   |
6  | -- This is a test for catching an unknown character
7  | -- The following line should fail
8  | 😄 := 42
   | ^^ this character is not allowed in Nerd code
9  | prn("Hello, World!")
10 | prn("Goodbye, cruel world!")
   |
note: Outside strings only ANSI characters are allowed, space and the tab 
      character.  However, UTF-8 characters are allowed within strings
help: Change this character to a valid ANSI character
```

The messages use different styles of pointers:

- `^` - single caret to show a location directly related to the error
-  `^^^^^^` - multiple carets to highlight a signal token directly related to the error
-  `~~~~~` - multiple tildes to highlight secondary tokens related to the error

# Stand out features

Nerd is intended to have a very simple syntax.  In fact there is only a single keyword for conditional branching and another for loops.  It also supports extensive in

## Inference

Nerd is designed to infer as much as possible, even from actions following a declaration.  For example, in this basic case:

```
-- Define a variable
a := 42
```

`a` is inferred to be a `i32` (default integer type).  The `:=` operator is actually two operators `:` and `=`.  A type can be insert between them if you need to override the inference.

Additionally, a value can be inferred from its use later on:

```
-- Struct defined here
Point :: struct {
	x: f32
	y: f32
}

-- Define a variable here
a := 42

-- Use the variable `a` to create an instance of `Point`,  `a` is now inferred
-- to be `f32` and not `i32`:
p := Point { x: a, y: 0 }
```


## Conditional branching

In C, there are multiple keywords for conditional branching: `if`, `?:`,  and `switch`.  In Nerd, we only have the `on` keyword:

```
-- Basic if statement
on <cond> => <expression>

-- Basic if/else statement
on <cond> => <expression> else <expression>

-- General form
on <cond> {
	<value 1> => <expression 1>
	<value 2> => <expression 2>
	...
	else => <expression 3>
}

-- Advanced form
on {
	<cond 1> => <expression 1>
	<cond 2> => <expression 2>
	...
	else => <fallback expression>
}

-- Pattern matching ([..] is destructuring)
on point {
	[ x, 0 ] => <expression involving x>
	[ 0, y ] => <expression involving y>
	[ x, y ] => <expression involving x and y>
}
```

## Naming only via binding

All functions, structs, unions, enums etc are nameless and can be used anywhere as values and/or types.  To name something an explicit binding occurs via the `:` operator.

For aliasing and constants use `<name> : <optional-type> : <type/value>`. 

For variable declaration use `<name> : <optional-type> = <value>`.

For undefined variables use `<name> : <optional-type>`.

