use mod std.print

greet :: fn (name: string) -> string {
    prn($"Hello, {name}!")
}

main :: fn () {
    greet("World")
}
¬
{
    "code": "0314",
    "message": "Missing return for function returning `string`",
    "source_file": "tests/errors/012-missing-function-return.e",
    "primary_location": {
        "line": 3,
        "column": 10
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 10,
            "length": 2,
            "message": "This function can reach the end without returning a `string` value"
        }
    ],
    "notes": [
        "Block-bodied functions with explicit return types must return a value before they end."
    ],
    "help": [
        "Add `return <expr>` to the function body or remove the explicit return type if the function should not produce a value."
    ]
}
