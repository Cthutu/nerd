Thing :: plex { value i32 }
impl Thing {
    bad :: 1
}
main :: fn () => 0
¬
{
  "message": "Type mismatch: expected `function method`, found `non-function binding`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 3,
    "column": 5
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 5,
      "length": 3,
      "message": "This expression has type `non-function binding`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

impl [T] Box[T] {
    init :: fn (value: T) -> Box {
        return { value }
    }
}

main :: fn () -> i32 {
    box := IntBox.init(42)
    return box.value
}
¬
{
  "message": "Impl function `init` cannot be called as an associated function",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 14,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 14,
      "column": 23,
      "length": 1,
      "message": "`init` is called through a type, so its return type must contain `Self`"
    }
  ],
  "notes": [],
  "help": [
    "Change the impl function return type to `Self`, `^Self`, or a type such as `?Self`, or call it through a receiver value."
  ]
}

¬
Counter :: plex {
    window_count i32
}

main :: fn () {
    c := Counter { window_count: 1 }
    _ := c.windows_count
}
¬
{
  "message": "Unknown member `windows_count` for `Counter`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 7,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 7,
      "column": 12,
      "length": 13,
      "message": "`Counter` has no field or method named `windows_count`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/methods.e",
      "line": 1,
      "column": 1,
      "length": 7,
      "message": "Type `Counter` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Did you mean `.window_count`?"
  ]
}

¬
Point :: plex {
    x i32
}

impl Point {
    describe :: fn (self: Self) => $"{self.x}"
}

main :: fn () {
    p := Point { x: 1 }
    _ := p.describ()
}
¬
{
  "message": "Unknown member `describ` for `Point`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 11,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 11,
      "column": 12,
      "length": 7,
      "message": "`Point` has no field or method named `describ`"
    },
    {
      "kind": "secondary",
      "source_file": "tests/errors/methods.e",
      "line": 1,
      "column": 1,
      "length": 5,
      "message": "Type `Point` is defined here"
    }
  ],
  "notes": [],
  "help": [
    "Did you mean `.describe`?"
  ]
}

¬
use test.private_method

main :: fn () {
    box := make_box(1)
    box.bump(1)
}
¬
{
  "message": "Method `bump` for `Box` is private",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 5,
    "column": 9
  },
  "references": [
    {
      "kind": "primary",
      "line": 5,
      "column": 9,
      "length": 4,
      "message": "`bump` exists for `Box`, but it is not visible from this module"
    },
    {
      "kind": "secondary",
      "source_file": "__REPO__/tests/mods/test/private_method.n",
      "line": 1,
      "column": 5,
      "length": 3,
      "message": "Type `Box` is defined here"
    }
  ],
  "notes": [
    "The method is defined in module `test.private_method`."
  ],
  "help": [
    "Mark `bump` as `pub` in that module, or call it from inside the module."
  ]
}

¬
Counter :: plex {
    value i32
}

impl Counter {
    increment :: fn (counter: ^Self) {
        counter.value += 1
    }
}

main :: fn () {
    Counter.increment()
}
¬
{
  "message": "Receiver method `increment` cannot be called through a type",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 12,
    "column": 22
  },
  "references": [
    {
      "kind": "primary",
      "line": 12,
      "column": 22,
      "length": 1,
      "message": "`increment` requires a receiver value"
    }
  ],
  "notes": [],
  "help": [
    "Call the method through a value, for example `value.increment()`."
  ]
}

¬
Resource :: plex {
    active bool
}

impl Resource {
    done :: fn (resource: ^Self) {
        resource.active = no
    }
}

main :: fn () {
    defer Resource.done()
}
¬
{
  "message": "Receiver method `done` cannot be called through a type",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 12,
    "column": 24
  },
  "references": [
    {
      "kind": "primary",
      "line": 12,
      "column": 24,
      "length": 1,
      "message": "`done` requires a receiver value"
    }
  ],
  "notes": [],
  "help": [
    "Call the method through a value, for example `value.done()`."
  ]
}

¬
Thing :: plex { value i32 }
impl Thing { idle :: fn (self: ^Self) {} }
main :: fn () {}
¬
{
  "message": "Unused parameter `self`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 2,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 26,
      "length": 4,
      "message": "This parameter is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `self` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
Render :: trait { render :: fn (self: ^Self) }
Thing :: plex { value i32 }
impl Render for Thing { render :: fn (self: ^Self) {} }
main :: fn () {}
¬
{
  "message": "Unused parameter `self`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 3,
    "column": 39
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 39,
      "length": 4,
      "message": "This parameter is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `self` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
Thing :: plex { value i32 }
impl Thing { idle :: fn (receiver: ^Self) {} }
main :: fn () {}
¬
{
  "message": "Unused parameter `receiver`",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 2,
    "column": 26
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 26,
      "length": 8,
      "message": "This parameter is never read"
    }
  ],
  "notes": [
    "Assigning to a variable does not count as using it."
  ],
  "help": [
    "Remove `receiver` or prefix the name with `_` if it is deliberately unused."
  ]
}

¬
Thing :: plex { value i32 }
impl Thing { read :: fn (_self: ^Self) -> i32 { return _self.value } }
main :: fn () {}
¬
{
  "message": "Used parameter `_self` marked as unused",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 2,
    "column": 56
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 56,
      "length": 5,
      "message": "This read uses `_self`"
    },
    {
      "kind": "secondary",
      "line": 2,
      "column": 26,
      "length": 5,
      "message": "`_self` is marked unused by its leading `_`"
    }
  ],
  "notes": [
    "Leading `_` names are reserved for bindings that are deliberately unused."
  ],
  "help": [
    "Rename `_self` without the leading `_` now that it is used."
  ]
}

¬
widgets :: use test.public_method
main :: fn () {
    widgets.Widget.get()
}
¬
{
  "message": "Receiver method `get` cannot be called through a type",
  "source_file": "tests/errors/methods.e",
  "primary_location": {
    "line": 3,
    "column": 23
  },
  "references": [
    {
      "kind": "primary",
      "line": 3,
      "column": 23,
      "length": 1,
      "message": "`get` requires a receiver value"
    }
  ],
  "notes": [],
  "help": [
    "Call the method through a value, for example `value.get()`."
  ]
}
