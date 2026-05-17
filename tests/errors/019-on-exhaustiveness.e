value: i32 = 2
main :: fn () => on value {
    1 => "one"
}
¬
{
    "message": "Value-producing block-form `on` expressions must be exhaustive",
    "source_file": "tests/errors/019-on-exhaustiveness.e",
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
