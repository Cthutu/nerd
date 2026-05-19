use test.private_method

main :: fn () {
    box := make_box(1)
    box.bump(1)
}
¬
{
    "message": "Method `bump` for `Box` is private",
    "source_file": "tests/errors/083-private-methods.e",
    "primary_location": {
        "line": 5,
        "column": 9
    },
    "references": [
        {
            "kind": "primary",
            "line": 5,
            "column": 9,
            "length": 4,
            "message": "`bump` exists for `Box`, but it is not visible from this module"
        }
    ],
    "notes": [
        "The method is defined in module `test.private_method`."
    ],
    "help": [
        "Mark `bump` as `pub` in that module, or call it from inside the module."
    ]
}
