assert on "missing_platform_key"

main :: fn () {}
¬
{
    "message": "Platform assertion failed for `missing_platform_key`",
    "source_file": "tests/errors/074-platform-assertions.e",
    "primary_location": {
        "line": 1,
        "column": 1
    },
    "references": [
        {
            "kind": "primary",
            "line": 1,
            "column": 1,
            "length": 6,
            "message": "This file requires the platform key to be enabled"
        }
    ],
    "notes": [],
    "help": [
        "Build for a matching platform, build mode, or define the key with `-Dmissing_platform_key`."
    ]
}
¬
