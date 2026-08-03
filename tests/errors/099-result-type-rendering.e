PixelLayer :: plex {
}

GfxError :: enum {
    Missing
}

get_layer :: fn () -> ^PixelLayer\GfxError {
    return Missing!
}

main :: fn () {
    layer := get_layer()
    value := layer[0]
}
¬
{
    "message": "Type mismatch: expected `array, slice, dynamic array, string, or pointer`, found `^PixelLayer\\GfxError`",
    "source_file": "tests/errors/099-result-type-rendering.e",
    "primary_location": {
        "line": 14,
        "column": 19
    },
    "references": [
        {
            "kind": "primary",
            "line": 14,
            "column": 19,
            "length": 1,
            "message": "This expression has type `^PixelLayer\\GfxError`"
        }
    ],
    "notes": [],
    "help": [
        "Change the expression or annotation so both sides use the same type."
    ]
}
