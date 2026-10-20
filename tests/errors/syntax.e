`
¬
{
  "message": "Unexpected character '`'",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 1,
      "message": "Unexpected character '`'"
    }
  ],
  "notes": [],
  "help": [
    "Review the input near the failing character."
  ]
}

¬
+42
¬
{
  "message": "Missing value before Plus `+`",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 1,
      "message": "Plus `+` cannot appear here"
    }
  ],
  "notes": [],
  "help": [
    "Insert a literal, parenthesized expression, or unary operator"
  ]
}

¬
63+
¬
{
  "message": "Missing value before EOF",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 4
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 4,
      "length": 0,
      "message": "EOF cannot appear here"
    }
  ],
  "notes": [],
  "help": [
    "Insert a literal, parenthesized expression, or unary operator"
  ]
}

¬
28 29
¬
{
  "message": "Missing operator before Integer",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 4
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 4,
      "length": 2,
      "message": "Integer starts a new expression here"
    }
  ],
  "notes": [],
  "help": [
    "Insert an operator such as +, -, *, /, or %% between values"
  ]
}

¬
@
¬
{
  "message": "Expected Symbol but found EOF",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 1,
      "message": "Found EOF here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
(1+2
¬
{
  "message": "Expected RightParen `)` but found EOF",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 5,
      "length": 0,
      "message": "Found EOF here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
1)
¬
{
  "message": "Unexpected RightParen `)` after expression",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 2
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 2,
      "length": 1,
      "message": "Found RightParen `)` here"
    }
  ],
  "notes": [
    "This right parenthesis does not match an opening parenthesis"
  ],
  "help": [
    "Add the missing opening parenthesis or remove the extra right parenthesis"
  ]
}

¬
main :: fn () {
    return 0
¬
{
  "message": "Expected RightBrace `}` but found EOF",
  "source_file": "tests/errors/syntax.e",
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
      "message": "This opening delimiter is not closed"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 13,
      "length": 0,
      "message": "Reached EOF while looking for RightBrace `}`"
    }
  ],
  "notes": [],
  "help": [
    "Add the missing closing delimiter for this block"
  ]
}

¬
impl Display for Point {
    show :: fn (self: Self) => "point"

Display :: trait {
    show :: fn (self: Self) -> string
}
¬
{
  "message": "Expected RightBrace `}` before declaration",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 4,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 1,
      "length": 7,
      "message": "A closing delimiter is needed before this declaration"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 24,
      "length": 1,
      "message": "This opening delimiter is not closed"
    }
  ],
  "notes": [],
  "help": [
    "Add the missing closing delimiter before this line"
  ]
}

¬
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
¬
{
  "message": "Symbol is too long",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 256,
      "message": "This symbol is too long"
    }
  ],
  "notes": [
    "Symbols must be 255 characters or fewer."
  ],
  "help": [
    "Use a shorter symbol name."
  ]
}

¬
main ::
¬
{
  "message": "Expected declaration or expression but found EOF",
  "source_file": "tests/errors/syntax.e",
  "primary_location": {
    "line": 1,
    "column": 8
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 8,
      "length": 0,
      "message": "Found EOF here"
    }
  ],
  "notes": [],
  "help": [
    "Expected a declaration or expression after '::', but found end of file"
  ]
}
