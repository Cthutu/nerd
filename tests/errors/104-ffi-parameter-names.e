ffi "c" strlen (^i8) -> usize
¬
{
    "message": "Expected Symbol but found Caret `^`",
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
    "notes": [
        "FFI parameters require a name before their type."
    ],
    "help": [
        "Write the parameter as `name: Type`."
    ]
}
