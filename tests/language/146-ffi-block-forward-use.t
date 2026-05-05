wrap :: fn () -> ^void {
    return c_malloc(1)
}

ffi "c" {
    c_malloc :: malloc (num_bytes: usize) -> ^void
    c_free   :: free (ptr: ^void)
}

main :: fn () -> i32 {
    ptr := wrap()
    return on ptr {
        nil => 1
        else => {
            c_free(ptr)
            break 0
        }
    }
}
¬
0
¬
¬
¬
