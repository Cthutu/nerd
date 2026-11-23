=== case: unused-ffi-library ===
ffi "nerd_unused_library_does_not_exist" {
    never_called :: nerd_unused_symbol_does_not_exist () -> i32
}
main :: fn () { prn("unused FFI does not link") }
¬
0
¬
unused FFI does not link
¬

¬
