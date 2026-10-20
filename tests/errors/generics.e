pair :: fn [A, B] (a: A, b: B) => a
main :: fn () => pair[i32](1, "two")
¬
{
  "message": "Argument count mismatch: expected 2, found 1",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 1,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 9,
      "length": 2,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 2 arguments to match the function signature."
  ]
}

¬
unused :: fn [T] (value: T) -> T {
    return missing_value(value)
}

main :: fn () => 0
¬
{
  "message": "Unknown symbol `missing_value`",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 13,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `missing_value` or fix the spelling."
  ]
}

¬
bad_eq :: fn [T] (lhs: T, rhs: T) -> bool {
    return lhs == rhs
}

main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
bad_order :: fn [T] (lhs: T, rhs: T) -> bool {
    return lhs < rhs
}

main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Order` constraint",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 1,
      "message": "This operation requires `T: Order`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Order` to the generic function or impl."
  ]
}

¬
add :: fn [T] (lhs: T, rhs: T) -> T {
    return lhs + rhs
}

main :: fn () {
    _ := add("a", "b")
}
¬
{
  "message": "Operator `+` requires matching numeric operands, found `string` and `string`",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 16
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 16,
      "length": 1,
      "message": "These operands have types `string` and `string`"
    }
  ],
  "notes": [],
  "help": [
    "Use `+` only with matching numeric operands."
  ]
}

¬
field :: fn [T] (value: T) -> i32 {
    return value.x
}

main :: fn () -> i32 {
    return field(42)
}
¬
{
  "message": "Type mismatch: expected `array, slice, string, dynamic array, module, plex, union, enum, or pointer to memberable value`, found `i32`",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 18,
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
impl [T] []T {
    contains :: fn (values: Self, expected: T) -> bool {
        return values[0] == expected
    }
}

main :: fn () => 0
¬
{
  "message": "Generic parameter `T` requires an `Eq` constraint",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 3,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 26,
      "length": 2,
      "message": "This operation requires `T: Eq`"
    }
  ],
  "notes": [],
  "help": [
    "Add `where T: Eq` to the generic function or impl."
  ]
}

¬
Iterator :: trait [Item] {
    next :: fn (self: Self) -> Item
}

first :: fn [T] (iter: T) -> T
where T: Iterator {
    return iter
}

main :: fn () => 0
¬
{
  "message": "Generic trait `Iterator` expects 1 type argument, found 0",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 6,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 10,
      "length": 8,
      "message": "Trait generic parameter `Item` cannot be inferred from this constraint"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/generics.e",
      "line": 1,
      "column": 1,
      "length": 8,
      "message": "Type `Iterator` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Provide exactly 1 explicit type argument for `Iterator`."
  ]
}

¬
Iterator :: trait [Item] {
    next :: fn (self: Self) -> Item
}

first :: fn [T] (iter: T) -> T
where T: Iterator[i32, string] {
    return iter
}

main :: fn () => 0
¬
{
  "message": "Generic trait `Iterator` expects 1 type argument, found 2",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 6,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 10,
      "length": 8,
      "message": "This trait constraint has the wrong number of type arguments"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/generics.e",
      "line": 1,
      "column": 1,
      "length": 8,
      "message": "Type `Iterator` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Provide exactly 1 explicit type argument for `Iterator`."
  ]
}

¬
Pair :: plex [T] {
    first T
}

Bad :: plex {
    use Pair[i32]
    use Pair[f32]
}

main :: fn () {}
¬
{
  "message": "Plex type `Pair` is embedded more than once",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 7,
    "column": 5
  },
  "references": [
    {
      "kind": "secondary",
      "source_file": "tests/errors/generics.e",
      "line": 1,
      "column": 1,
      "length": 4,
      "message": "Type `Pair` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Each plex may use a generic type declaration only once."
  ]
}


¬
pair :: fn [A, B] (a: A, b: B) -> A { _ := b
 return a }
main :: fn () { _ := pair[i32](1, 2) }
¬
{
  "message": "Argument count mismatch: expected 2, found 1",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 1,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 9,
      "length": 2,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 2 arguments to match the function signature."
  ]
}

¬
pair :: fn [A, B] (a: A, b: B) -> A { _ := b
 return a }
main :: fn () { _ := pair[i32,](1, 2) }
¬
{
  "message": "Missing value before RightBracket `]`",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 3,
    "column": 31
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 31,
      "length": 1,
      "message": "RightBracket `]` cannot appear here"
    }
  ],
  "notes": [],
  "help": [
    "Insert a literal, parenthesized expression, or unary operator"
  ]
}

¬
Cell :: plex [T] { value T }
main :: fn () { _ := Cell[i32, string].init(1) }
¬
{
  "message": "Argument count mismatch: expected 1, found 2",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 26,
      "length": 1,
      "message": "This call uses the wrong arity"
    }
  ],
  "notes": [],
  "help": [
    "Pass exactly 1 argument to match the function signature."
  ]
}

¬
Cell :: plex [T] { value T }
main :: fn () { _ := Cell[i32].missing(1) }
¬
{
  "message": "Unknown member `missing` for `plex { i32 value }`",
  "source_file": "tests/errors/generics.e",
  "primary_location": {
    "line": 2,
    "column": 32
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 32,
      "length": 7,
      "message": "`plex { i32 value }` has no field or method named `missing`"
    }
  ],
  "notes": [],
  "help": [
    "Use a field or method that exists on `plex { i32 value }`."
  ]
}
