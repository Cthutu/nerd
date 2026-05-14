-- test-platform: windows
ffi "user32" {
    GetModuleHandle ()
}

main :: fn () {}
¬
{
    "code": "0346",
    "message": "Foreign symbol `GetModuleHandle` was not found in `user32`",
    "source_file": "tests/errors/072-windows-ffi-symbols.e",
    "primary_location": {
        "line": 3,
        "column": 5
    },
    "references": [
        {
            "kind": "primary",
            "line": 3,
            "column": 5,
            "length": 15,
            "message": "This FFI declaration links to `GetModuleHandle`"
        }
    ],
    "notes": [
        "Windows API names that are macros in C headers often need an explicit `A` or `W` foreign symbol name in Nerd."
    ],
    "help": [
        "Use `local_name :: foreign_name (...)` with the real exported symbol, or move the declaration to the library that exports it."
    ]
}
