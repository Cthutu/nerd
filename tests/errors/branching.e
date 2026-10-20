main :: fn () => on 1.as(i32) => 42 else 7
¬
{
  "message": "`on` condition requires bool, box, optional or result, found `i32`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 1,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 22,
      "length": 1,
      "message": "This expression cannot be used as a condition"
    }
  ],
  "notes": [],
  "help": [
    "Use bool or box/optional/result presence, or block-form `on` for value matching."
  ]
}

¬
enabled: bool = yes
main :: fn () => on enabled => 42 else "no"
¬
{
  "message": "`on` branches must return the same type",
  "source_file": "tests/errors/branching.e",
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
      "message": "This branch has type `string`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 32,
      "length": 2,
      "message": "The other branch has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Make both branches produce exactly the same type."
  ]
}

¬
message :: 3.14
main :: fn () => on message {
    else => 1
}
¬
{
  "message": "Block-form `on` does not support values of type `untyped float`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 21,
      "length": 7,
      "message": "This matched value type is unsupported"
    }
  ],
  "notes": [],
  "help": [
    "Block-form `on` supports `bool` and `string` scrutinees, plus concrete integer scrutinees."
  ]
}

¬
value: i32 = 2
main :: fn () => on value {
    _ => 10
}
¬
{
  "message": "Block-form `on` wildcard pattern must use `else`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 3,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 5,
      "length": 1,
      "message": "This wildcard matches every value"
    }
  ],
  "notes": [],
  "help": [
    "Use an `else` branch instead of `_` as a top-level value pattern."
  ]
}

¬
value: i32 = 2
main :: fn () => on value {
    2..2 => 10
    else => 20
}
¬
{
  "message": "Block-form `on` range pattern is empty",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 3,
    "column": 6
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 6,
      "length": 2,
      "message": "This range cannot match any values"
    }
  ],
  "notes": [],
  "help": [
    "Use a lower bound that is strictly less than the upper bound for `..` ranges."
  ]
}

¬
value: i32 = 2
main :: fn () => on value {
    1 => "one"
}
¬
{
  "message": "Value-producing block-form `on` expressions must be exhaustive",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 18,
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
value: i32 = 2
main :: fn () => on value {
    1 on value => 1
    else => 2
}
¬
{
  "message": "`on` condition requires bool, box, optional or result, found `i32`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 3,
    "column": 10
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 10,
      "length": 5,
      "message": "This expression cannot be used as a condition"
    }
  ],
  "notes": [],
  "help": [
    "Use bool or box/optional/result presence, or block-form `on` for value matching."
  ]
}

¬
value: i32 = 2
main :: fn () => on value {
    1 on no => 1
}
¬
{
  "message": "Value-producing block-form `on` expressions must be exhaustive",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 18,
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
main :: fn () => on (1, 2) {
    (1, 2, 3) => 1
    else => 0
}
¬
{
  "message": "Type mismatch: expected `tuple with matching arity`, found `(i32, i32)`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
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
Point :: plex { x i32 y i32 }
main :: fn () => on Point { x: 1, y: 2 } {
    { z: 3 } => 1
    else => 0
}
¬
{
  "message": "Unknown field `z` in plex pattern",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 3,
    "column": 7
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 7,
      "length": 1,
      "message": "No field named `z` exists on the matched plex type"
    }
  ],
  "notes": [],
  "help": [
    "If you meant to bind an existing field to a differently named local, write `field: binding`. Pattern shorthand only works when the field and binding have the same name."
  ]
}

¬
main :: fn () => on (1, 2) {
    (1, for x) => 1
    else => 0
}
¬
{
  "message": "Unknown symbol `x`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 13
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 13,
      "length": 1,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `x` or fix the spelling."
  ]
}

¬
Point :: plex { x i32 }
Other :: plex { x i32 }

main :: fn () -> i32 {
    point := Point { x: 1 }
    return on point {
        Other { x: 1 } => 1
        else => 0
    }
}
¬
{
  "message": "Type mismatch: expected `Point`, found `Other`",
  "source_file": "tests/errors/branching.e",
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
      "source_file": "tests/errors/branching.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/branching.e",
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
main :: fn () -> i32 {
    value := 2
    return on {
        value < 0 => -1
        value > 0 => 1
    }
}
¬
{
  "message": "Value-producing block-form `on` expressions must be exhaustive",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 3,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
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
main :: fn () -> i32 {
    value := 2
    return on {
        value => 1
        else => 0
    }
}
¬
{
  "message": "`on` condition requires bool, box, optional or result, found `i32`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 5,
      "message": "This expression cannot be used as a condition"
    }
  ],
  "notes": [],
  "help": [
    "Use bool or box/optional/result presence, or block-form `on` for value matching."
  ]
}

¬
main :: fn () -> i32 {
    value := "hi"
    return on value {
        > "bye" => 1
        else => 0
    }
}
¬
{
  "message": "Operator `>` requires matching numeric operands, found `string` and `string`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 4,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 4,
      "column": 9,
      "length": 1,
      "message": "These operands have types `string` and `string`"
    }
  ],
  "notes": [],
  "help": [
    "Use `>` only with matching numeric operands."
  ]
}

¬
on 1 {}
¬
{
  "message": "Expected String but found Integer",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 1,
    "column": 4
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 4,
      "length": 1,
      "message": "Found Integer here"
    }
  ],
  "notes": [],
  "help": [
    "Check for a missing closing delimiter or misplaced operator"
  ]
}

¬
on !"debug" {
    answer :: 7
}

main :: fn () => answer
¬
{
  "message": "Unknown symbol `answer`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 5,
    "column": 18
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 18,
      "length": 6,
      "message": "This symbol is not defined"
    }
  ],
  "notes": [],
  "help": [
    "Add a binding for `answer` or fix the spelling."
  ]
}

¬
get_result :: fn () -> i32\string {
    return 1
}

main :: fn () {
    on get_result() [value] {
        value
    }
}
¬
{
  "message": "Missing `=>` before `on` extraction binder",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 6,
    "column": 21
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 21,
      "length": 6,
      "message": "This looks like a success payload binder"
    },
    {
      "kind": "secondary",
      "line": 8,
      "column": 5,
      "length": 1,
      "message": "The missing arrow made this parse as indexing"
    }
  ],
  "notes": [],
  "help": [
    "Write `on value => [payload] { ... } else [error] { ... }`"
  ]
}

¬
get_result :: fn () -> i32\string {
    return 1
}

main :: fn () {
    on get_result()[value] {
    }
}
¬
{
  "message": "Missing `=>` before `on` extraction binder",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 6,
    "column": 20
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 20,
      "length": 6,
      "message": "This looks like a success payload binder"
    }
  ],
  "notes": [
    "Without `=>`, `[payload]` is parsed as indexing"
  ],
  "help": [
    "Write `on value => [payload] { ... } else [error] { ... }`"
  ]
}

¬
Values :: [10, 20]

main :: fn () {
    enabled := yes
    value := on enabled => {
        Values[0]
    } else {
        Values[1]
    }
    _ := value
}
¬
{
  "message": "`on` expression does not produce a value",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 5,
    "column": 14
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 14,
      "length": 2,
      "message": "This `on` expression has type `void`"
    },
    {
      "kind": "secondary",
      "line": 5,
      "column": 28,
      "length": 1,
      "message": "This braced branch is a statement block"
    }
  ],
  "notes": [],
  "help": [
    "Write each result expression directly, or use an expression block `${ ... }` with `break <value>` in each branch."
  ]
}

¬
main :: fn () -> bool {
    left := 1
    right := 2
    on left != right {
        return no
    }
    return yes
}
¬
{
  "message": "Missing value before Keyword `return`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 5,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 9,
      "length": 6,
      "message": "Keyword `return` cannot appear here"
    }
  ],
  "notes": [],
  "help": [
    "If you intended an if-like conditional, add `=>` before the block: `on condition => { ... }`"
  ]
}

¬
main :: fn () {
    n system.poll(^frame) {
        None => 0
    }
}
¬
{
  "message": "Unexpected `n` before match expression",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 2,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 5,
      "length": 1,
      "message": "`n` is parsed as a separate expression statement"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 7,
      "length": 6,
      "message": "This looks like the value to match"
    }
  ],
  "notes": [],
  "help": [
    "Replace `n` with `on`"
  ]
}

¬
main :: fn () {
    on yes => [value] {
        _ := value
    }
}
¬
{
  "message": "Cannot extract an `on` payload from `bool`",
  "source_file": "tests/errors/branching.e",
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
      "message": "This binder requires a success or error payload"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 8,
      "length": 3,
      "message": "This expression has type `bool`"
    }
  ],
  "notes": [],
  "help": [
    "Remove the extraction binder to branch on this value, or match an optional or result value that carries a payload."
  ]
}

¬
Key :: enum { Q Escape }
Event :: enum { Press { key Key } }
main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q }, Press { key: ESC } => {}
    }
}
¬
{
  "message": "Type mismatch: expected `known enum variant`, found `ESC`",
  "source_file": "tests/errors/branching.e",
  "primary_location": {
    "line": 6,
    "column": 40
  },
  "references": [
    {
      "kind": "primary",
      "line": 6,
      "column": 40,
      "length": 3,
      "message": "This expression has type `ESC`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}
