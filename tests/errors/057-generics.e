pair :: fn [A, B] (a: A, b: B) => a
main :: fn () => pair[i32](1, "two")
¬
{
    "message": "Argument count mismatch: expected 2, found 1",
    "source_file": "tests/errors/057-generics.e",
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
