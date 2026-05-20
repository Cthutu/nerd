use test.folder_pub_use.child

main :: fn () {}
¬
{
    "message": "Type mismatch: expected `known module`, found `module path`",
    "source_file": "tests/errors/084-folder-module-boundary.e",
    "primary_location": {
        "line": 1,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 5,
            "length": 4,
            "message": "This expression has type `module path`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
