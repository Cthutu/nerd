Hash :: plex { a u32 }
impl Display for Hash {
 other :: fn (self: Self) -> string {
  show :: fn (value: u32) -> string { return $"{value:02X}" }
  return show(self.a)
 }
}
main :: fn () { hash := Hash { a: 10 } assert hash.a == 10 }
¬
{
    "message": "Trait implementation is missing required member",
    "source_file": "tests/errors/impl-local-bindings.e",
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
            "message": "This implementation does not define every member required by `Display`"
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
    "notes": [
        "Missing member: `show`"
    ],
    "help": [
        "Add the missing member to this `impl` block."
    ]
}
¬
Hash :: plex { a u32 }
impl Hash {
 word :: fn (self: Self) -> u32 {
  helper :: fn (value: u32) -> u32 { return value + 1 }
  return helper(self.a)
 }
}
main :: fn () { hash := Hash { a: 10 } assert hash.helper(1) == 2 }
¬
{
    "message": "Unknown member `helper` for `Hash`",
    "source_file": "tests/errors/impl-local-bindings.e",
    "primary_location": {
        "line": 8,
        "column": 52
    },
    "references": [
        {
            "kind": "primary",
            "line": 8,
            "column": 52,
            "length": 6,
            "message": "`Hash` has no field or method named `helper`"
        },
        {
            "kind": "secondary",
            "source_file": "tests/errors/impl-local-bindings.e",
            "line": 1,
            "column": 1,
            "length": 4,
            "message": "Type `Hash` is defined here"
        }
    ],
    "notes": [],
    "help": [
        "Use a field or method that exists on `Hash`."
    ]
}
¬
Hash :: plex { a u32 }
impl Display for Hash {
 show :: fn (self: ^Self) -> string {
  helper :: fn (value: u32) -> string { return "unused" }
  return helper(self.a)
 }
}
main :: fn () { hash := Hash { a: 10 } assert hash.show() == "unused" }
¬
{
    "message": "Unused parameter `value`",
    "source_file": "tests/errors/impl-local-bindings.e",
    "primary_location": {
        "line": 4,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 4,
            "column": 17,
            "length": 5,
            "message": "This parameter is never read"
        }
    ],
    "notes": [
        "Assigning to a variable does not count as using it."
    ],
    "help": [
        "Remove `value` or prefix the name with `_` if it is deliberately unused."
    ]
}
