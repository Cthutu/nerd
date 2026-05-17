RoomType :: enum { NONE }
Room :: plex {
    exits ["many"]RoomType
}
main :: fn () {}
¬
{
    "message": "Type mismatch: expected `non-negative integer constant`, found `string`",
    "source_file": "tests/errors/043-array-length-constants.e",
    "primary_location": {
        "line": 2,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 2,
            "column": 1,
            "length": 4,
            "message": "This expression has type `string`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
