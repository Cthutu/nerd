main :: fn () -> i32 {
    _result := 1
    return _result
}
¬
{
    "message": "Used local variable `_result` marked as unused",
    "source_file": "tests/errors/073-used-underscore-local.e",
    "primary_location": {
        "line": 3,
        "column": 12
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 12,
            "length": 7,
            "message": "This read uses `_result`"
        },
        {
            "kind": "secondary",
            "line": 2,
            "column": 5,
            "length": 7,
            "message": "`_result` is marked unused by its leading `_`"
        }
    ],
    "notes": [
        "Leading `_` names are reserved for bindings that are deliberately unused."
    ],
    "help": [
        "Rename `_result` without the leading `_` now that it is used."
    ]
}
