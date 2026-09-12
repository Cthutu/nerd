ffi "c" strlen (^i8) -> usize
¬
{
    "message": "Expected declaration or expression but found Caret `^`",
    "source_file": "tests/errors/104-ffi-parameter-names.e",
    "primary_location": {
        "line": 1,
        "column": 17
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 17,
            "length": 1,
            "message": "Found Caret `^` here"
        }
    ],
    "notes": [],
    "help": [
        "Function parameters require names; write `name: Type`."
    ]
}
