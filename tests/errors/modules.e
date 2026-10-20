a :: b + 1
b :: a + 1
¬
{
  "message": "Dependency cycle involving `b`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 1,
      "length": 1,
      "message": "This binding participates in a dependency cycle"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 1,
      "length": 1,
      "message": "Cycle closes through `a` here"
    }
  ],
  "notes": [],
  "help": [
    "Break the cycle by rewriting one of the bindings so it no longer depends on the other."
  ]
}

¬
Price :: Cost
Cost :: Price
¬
{
  "message": "Type alias cycle involving `Cost`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 1,
      "length": 4,
      "message": "This alias participates in a type cycle"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Cycle closes through `Price` here"
    }
  ],
  "notes": [],
  "help": [
    "Break the cycle by rewriting one of the aliases so it resolves to a concrete type."
  ]
}

¬
bad :: use std.missing

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `known module`, found `module path`",
  "source_file": "tests/errors/modules.e",
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
      "message": "This expression has type `module path`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
value :: 1
use value

main :: fn() {}
¬
{
  "message": "Type mismatch: expected `known module`, found `module path`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 5,
      "message": "This expression has type `module path`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
box :: use test.reexport

main :: fn() {
    box.secret()
}
¬
{
  "message": "Type mismatch: expected `known module export`, found `secret`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 6,
      "message": "This expression has type `secret`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
forty_two :: 0
use test.reexport

main :: fn() => forty_two
¬
{
  "message": "Duplicate binding for symbol `forty_two`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 1,
      "length": 3,
      "message": "This binding redefines `forty_two`"
    },
    {
      "kind": "secondary",
      "line": 1,
      "column": 1,
      "length": 9,
      "message": "Previous binding of `forty_two` is here"
    }
  ],
  "notes": [],
  "help": [
    "Rename one of the bindings or remove the duplicate definition."
  ]
}

¬
part body
¬
{
  "message": "Missing operator before Symbol",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 1,
    "column": 6
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 6,
      "length": 4,
      "message": "Symbol starts a new expression here"
    }
  ],
  "notes": [],
  "help": [
    "Insert an operator such as +, -, *, /, or %% between values"
  ]
}

¬
use test.folder_pub_use.child

main :: fn () {}
¬
{
  "message": "Type mismatch: expected `known module`, found `module path`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 1,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 5,
      "length": 4,
      "message": "This expression has type `module path`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
use private_hint_a
use private_hint_b
main :: fn () { _value: VertexArray }
¬
{
  "message": "Unknown type `VertexArray`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 3,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 25,
      "length": 11,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types.",
    "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?",
    "Used module `private_hint_b` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
  ]
}

¬
use private_hint_a
use private_hint_b
main :: fn () { _value := secret_value }
¬
{
  "message": "Unknown symbol `secret_value`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 3,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 27,
      "length": 12,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `secret_value` or fix the spelling.",
    "Used module `private_hint_a` declares `secret_value` privately. Did you intend to mark that declaration `pub`?",
    "Used module `private_hint_b` declares `secret_value` privately. Did you intend to mark that declaration `pub`?"
  ]
}

¬
a :: use private_hint_a
b :: use private_hint_a
main :: fn () { _value: VertexArray }
¬
{
  "message": "Unknown type `VertexArray`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 3,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 25,
      "length": 11,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types.",
    "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
  ]
}

¬
use private_hint_a
main :: fn () { _value: Hidden }
¬
{
  "message": "Unknown type `Hidden`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 6,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
use private_hint_a
main :: fn () { _value: ValueOnly }
¬
{
  "message": "Unknown type `ValueOnly`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 25
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 25,
      "length": 9,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types."
  ]
}

¬
a :: use private_hint_a
main :: fn () { _value: a.VertexArray }
¬
{
  "message": "Unknown type `VertexArray`",
  "source_file": "tests/errors/modules.e",
  "primary_location": {
    "line": 2,
    "column": 27
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 27,
      "length": 11,
      "message": "This type name is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Use a defined type name, or one of the built-in primitive types.",
    "Used module `private_hint_a` declares `VertexArray` privately. Did you intend to mark that declaration `pub`?"
  ]
}
