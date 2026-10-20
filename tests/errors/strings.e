"Hello, world!
¬
{
  "message": "Unterminated string literal",
  "source_file": "tests/errors/strings.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 14,
      "message": "String literal starts here"
    }
  ],
  "notes": [],
  "help": [
    "Add a closing double quote to terminate the string literal.",
    "Use `+\"...\"` for an intentional string continuation."
  ]
}

¬
count := 3
message := $"count={count}"
¬
{
  "message": "Runtime interpolated strings cannot be top-level values",
  "source_file": "tests/errors/strings.e",
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
      "message": "This interpolation needs runtime string building"
    }
  ],
  "notes": [],
  "help": [
    "Use only compile-time values in top-level interpolated strings, or move the interpolation into a function."
  ]
}

¬
use std.io

helper :: fn () => 1
main :: fn () {
    prn($"helper={helper}")
}
¬
{
  "message": "Type mismatch: expected `Display implementation`, found `fn () -> i32`",
  "source_file": "tests/errors/strings.e",
  "primary_location": {
    "line": 5,
    "column": 19
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 19,
      "length": 6,
      "message": "This expression has type `fn () -> i32`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/mods/core.n",
      "line": 198,
      "column": 5,
      "length": 7,
      "message": "Type `Display` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () { pr() }
¬
{
  "message": "Argument count mismatch: expected 1, found 0",
  "source_file": "tests/errors/strings.e",
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
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `text`."
  ]
}

¬
main :: fn () { value := temp_arena.pr() }
¬
{
  "message": "Argument count mismatch: expected 1, found 0",
  "source_file": "tests/errors/strings.e",
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
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `text`."
  ]
}

¬
main :: fn () { epr() }
¬
{
  "message": "Argument count mismatch: expected 1, found 0",
  "source_file": "tests/errors/strings.e",
  "primary_location": {
    "line": 1,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 20,
      "length": 1,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `text`."
  ]
}

¬
main :: fn () { value := temp_arena.prn() }
¬
{
  "message": "Argument count mismatch: expected 1, found 0",
  "source_file": "tests/errors/strings.e",
  "primary_location": {
    "line": 1,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 40,
      "length": 1,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass an argument for parameter `text`."
  ]
}
