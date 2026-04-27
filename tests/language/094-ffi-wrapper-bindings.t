c_realloc :: ffi "c" realloc (^void, usize) -> ^void

realloc :: fn (ptr: ^void, size: usize) -> ^void {
    return c_realloc(ptr, size)
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬

¬
