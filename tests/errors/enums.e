main :: fn () {
    value :: Red
}
¬
{
  "message": "Unknown symbol `Red`",
  "source_file": "tests/errors/enums.e",
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
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `Red` or fix the spelling."
  ]
}

¬
Colour :: enum { Red Green }

main :: fn () {
    value: Colour = Blue
}
¬
{
  "message": "Type mismatch: expected `known enum variant`, found `Blue`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 4,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 21,
      "length": 4,
      "message": "This expression has type `Blue`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Red Green Blue }

main :: fn () -> i32 {
    value: Colour = Red
    return on value {
        Red => 1
        Green => 2
    }
}
¬
{
  "message": "Value-producing block-form `on` expressions must be exhaustive",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 5,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 12,
      "length": 2,
      "message": "This `on` does not produce a value on every path"
    }
  ],
  "notes": [],
  "help": [
    "Add an `else` branch, or use this `on` as a statement when missing cases should be a no-op."
  ]
}

¬
Colour :: enum { Red Green }

main :: fn () {
    value := Colour.Blue
}
¬
{
  "message": "Type mismatch: expected `known enum variant`, found `Blue`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 4,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 21,
      "length": 4,
      "message": "This expression has type `Blue`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Maybe :: enum { None Some(i32) }

main :: fn () {
    value: Maybe = Some("bad")
}
¬
{
  "message": "Type mismatch: expected `i32`, found `string`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 4,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 25,
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
Event :: enum {
    Resized {
        width u16
        height u16
    }
}

main :: fn () {
    event := Event.Resized { width: 1, height: 2 }
    on event {
        Resized(width, height) => {}
    }
}
¬
{
  "message": "Enum payload pattern is missing field names",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 11,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 11,
      "column": 9,
      "length": 7,
      "message": "This variant has a braced payload with named fields"
    }
  ],
  "notes": [],
  "help": [
    "Name the fields in the pattern, for example `Resized { field: binding }`. If the binding name is the same as the field name, `Resized { field }` is shorthand."
  ]
}

¬
Maybe :: enum { None Some(i32) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        None => 0
        Some(x, y) => x + y
    }
}
¬
{
  "message": "Argument count mismatch: expected 1, found 2",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 7,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 7,
      "column": 9,
      "length": 4,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 1 argument to match the function signature."
  ]
}

¬
Maybe :: enum { None Some(i32) }
Other :: enum { None Some(i64) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Other.Some(x) => x
        else => 0
    }
}
¬
{
  "message": "Type mismatch: expected `Maybe`, found `Other`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 7,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 7,
      "column": 9,
      "length": 5,
      "message": "This expression has type `Other`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Maybe` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 2,
      "column": 1,
      "length": 5,
      "message": "Type `Other` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Maybe :: enum { None Some(i32) }

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Some(x), None => x
    }
}
¬
{
  "message": "Type mismatch: expected `non-payload enum variant`, found `Some`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 6,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 9,
      "length": 4,
      "message": "This expression has type `Some`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Mode :: enum {
    Read = 1
    Write = 1
}

main :: fn () {}
¬
{
  "message": "Duplicate enum discriminant value `1`",
  "source_file": "tests/errors/enums.e",
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
      "message": "This variant also uses value `1`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 12,
      "length": 1,
      "message": "Previous variant with value `1` is here"
    }
  ],
  "notes": [],
  "help": [
    "Give one of the variants a distinct explicit value, or remove the explicit value so the implicit sequence does not collide."
  ]
}

¬
Mode :: enum {
    Read
    Write(i32)
    Read(string)
}

main :: fn () {}
¬
{
  "message": "Duplicate enum variant `Read`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 4,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 5,
      "length": 4,
      "message": "This variant reuses `Read`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 5,
      "length": 4,
      "message": "Previous variant `Read` is here"
    }
  ],
  "notes": [],
  "help": [
    "Rename one of the variants so every variant in the enum is unique."
  ]
}

¬
Mode :: enum {
    Read = 1
    Write = 0
    Execute
}

main :: fn () {}
¬
{
  "message": "Duplicate enum discriminant value `1`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 4,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 5,
      "length": 7,
      "message": "This variant also uses value `1`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 12,
      "length": 1,
      "message": "Previous variant with value `1` is here"
    }
  ],
  "notes": [],
  "help": [
    "Give one of the variants a distinct explicit value, or remove the explicit value so the implicit sequence does not collide."
  ]
}

¬
Mode :: enum {
    Fixed {
        width  u16
        height u16
    }
}

main :: fn () {
    mode := Mode.Fixed(400, 300)
}
¬
{
  "message": "Enum variant with named fields requires braced syntax",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 9,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 9,
      "column": 23,
      "length": 1,
      "message": "This variant has a braced payload with named fields"
    }
  ],
  "notes": [],
  "help": [
    "Construct it as `Fixed { field: value }` instead of calling it with `(...)`."
  ]
}

¬
Colour :: enum { Red Green = 4 Blue }
main :: fn () -> i32 { return Colour.Red }

¬
{
  "message": "Type mismatch: expected `i32`, found `Colour`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 3,
      "message": "This expression has type `Colour`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "Type `Colour` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Red Green = 4 Blue }
main :: fn () -> i32 { return Colour.Blue }

¬
{
  "message": "Type mismatch: expected `i32`, found `Colour`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
      "length": 4,
      "message": "This expression has type `Colour`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "Type `Colour` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Green = 4 }
main :: fn () -> i32 { colour := Colour.Green return colour }

¬
{
  "message": "Type mismatch: expected `i32`, found `Colour`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 54
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 54,
      "length": 6,
      "message": "This expression has type `Colour`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "Type `Colour` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Green = 4 }
green: Colour: Colour.Green
main :: fn () -> i32 { return green }

¬
{
  "message": "Type mismatch: expected `i32`, found `Colour`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 3,
    "column": 31
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 31,
      "length": 5,
      "message": "This expression has type `Colour`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "Type `Colour` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Value :: enum { None Some(i32) = 4 }
main :: fn () -> i32 { return Value.Some(42) }

¬
{
  "message": "Type mismatch: expected `i32`, found `Value`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 41
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 41,
      "length": 1,
      "message": "This expression has type `Value`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/enums.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Value` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Green = 256 }
main :: fn () { _number: u8 = Colour.Green }

¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
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
Colour :: enum { Green = 128 }
main :: fn () { _number: i8 = Colour.Green }

¬
{
  "message": "Type mismatch: expected `i8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
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
Colour :: enum { Green = 4294967295 }
main :: fn () -> i32 { return Colour.Green }

¬
{
  "message": "Type mismatch: expected `i32`, found `out-of-range integer literal`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 38
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 38,
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
Colour :: enum { Green = 256 }
green :: Colour.Green
main :: fn () { _number: u8 = green }

¬
{
  "message": "Type mismatch: expected `u8`, found `out-of-range integer literal`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 3,
    "column": 31
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 31,
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
other :: use enum_integer_contexts
main :: fn () -> i32 { return other.Colour.Blue }

¬
{
  "message": "Type mismatch: expected `i32`, found `Colour`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 2,
    "column": 44
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 44,
      "length": 4,
      "message": "This expression has type `Colour`"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/tests/mods/enum_integer_contexts.n",
      "line": 1,
      "column": 5,
      "length": 6,
      "message": "Type `Colour` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Colour :: enum { Green = 4 }
wide :: fn (value: i32) => value
small :: fn (value: u8) => value
choose :: fn { wide small }
main :: fn () { _value := choose(Colour.Green) }

¬
{
  "message": "Type mismatch: expected `exactly one compatible compound function member`, found `ambiguous compound call`",
  "source_file": "tests/errors/enums.e",
  "primary_location": {
    "line": 5,
    "column": 33
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 33,
      "length": 1,
      "message": "This expression has type `ambiguous compound call`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}


¬
Maybe :: enum {
    None
    Some(i32)
}

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Some(as code) => code
        None => 0
    }
}
¬
{
    "message": "Expected Symbol but found Keyword `as`",
    "source_file": "tests/errors/enums.e",
    "primary_location": {
        "line": 9,
        "column": 14
    },
    "references": [
        {
            "kind": "primary",
            "line": 9,
            "column": 14,
            "length": 2,
            "message": "Found Keyword `as` here"
        }
    ],
    "notes": [],
    "help": [
        "Check for a missing closing delimiter or misplaced operator"
    ]
}
