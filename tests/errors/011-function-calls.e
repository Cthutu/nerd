add :: fn (a: i32, b: i32) => a + b
main :: fn () => add(20)
¬
{
    "code": "0313",
    "message": "Argument count mismatch: expected 2, found 1",
    "source_file": "tests/errors/011-function-calls.e",
    "primary_location": {
        "line": 2,
        "column": 21
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 21,
            "length": 1,
            "message": "This call uses the wrong arity"
        }
    ],
    "notes": [],
    "help": [
        "Pass exactly 2 arguments to match the function signature."
    ]
}
