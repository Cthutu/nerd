add :: fn (a: i32, b: i32 = 2) => a + b
main :: fn () => add(1, 3)
¬
{
    "message": "Defaulted parameter `b` requires a named argument",
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
    "source_file": "tests/errors/133-named-default-parameters.e",
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
