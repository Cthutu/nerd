text :: "hi"
value :: text.bytes
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `array or slice field `.bytes``, found `string`",
    "source_file": "tests/errors/087-bytes.e",
    "primary_location": {
        "line": 2,
        "column": 15
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 15,
            "length": 5,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
