add :: fn (a: i32, b: i32) -> i32 => a + b
main :: fn () => add(20, 22)
¬
{
    "code": "0318",
    "message": "Cannot combine an explicit return type with a fat-arrow body",
    "source_file": "tests/errors/014-invalid-function-forms.e",
    "primary_location": {
        "line": 1,
        "column": 8
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 8,
            "length": 2,
            "message": "This function uses `->` for an explicit return type and `=>` for an expression body"
        }
    ],
    "notes": [
        "Fat-arrow functions infer their return type from the body expression."
    ],
    "help": [
        "Use `fn (...) => expr` for inferred returns, or rewrite the function as `fn (...) -> T { return expr }`."
    ]
}
