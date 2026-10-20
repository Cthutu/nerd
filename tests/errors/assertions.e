main :: fn () {
    assert 1
}
¬
{
  "message": "Type mismatch: expected `bool`, found `untyped integer`",
  "source_file": "tests/errors/assertions.e",
  "primary_location": {
    "line": 2,
    "column": 12
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 12,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
main :: fn () {
    assert yes, 1
}
¬
{
  "message": "Type mismatch: expected `string`, found `untyped integer`",
  "source_file": "tests/errors/assertions.e",
  "primary_location": {
    "line": 2,
    "column": 17
  },
  "references": [
    {
      "kind": "primary",
      "line": 2,
      "column": 17,
      "length": 1,
      "message": "This expression has type `untyped integer`"
    }
  ],
  "notes": [],
  "help": [
    "Change the expression or annotation so both sides use the same type."
  ]
}

¬
assert on "missing_platform_key"

main :: fn () {}
¬
{
  "message": "Platform assertion failed for `missing_platform_key`",
  "source_file": "tests/errors/assertions.e",
  "primary_location": {
    "line": 1,
    "column": 1
  },
  "references": [
    {
      "kind": "primary",
      "line": 1,
      "column": 1,
      "length": 6,
      "message": "This file requires the platform key to be enabled"
    }
  ],
  "notes": [],
  "help": [
    "Build for a matching platform, build mode, or define the key with `-Dmissing_platform_key`."
  ]
}
