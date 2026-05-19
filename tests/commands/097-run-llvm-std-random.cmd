use std.mem

main :: fn () -> i32 {
    bytes := alloc(8)
    defer free(bytes)
    on alloc_size(bytes) != 8 => return 1
    return 0
}
¬
0
¬

¬
delete
