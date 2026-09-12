ffi "c" vprintf (format: ^i8, args: VaList) -> i32
use std.text
forward :: fn (format: ^i8, args: ...) {
    assert format_c(format, args) == "value=42"
    duplicate := args.copy()
    _count := vprintf(format, duplicate)
    assert args.next[i32]() == 42
}
main :: fn () { forward(c"value=%d", 42) }
¬
0
¬
value=42
