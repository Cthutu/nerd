use std.mem

main :: fn () -> i32 {
    bytes := alloc(8)
    on bytes.data == nil => return 1
    on alloc_size(bytes) != 8 => return 2

    bytes.data[0] = 42
    grown := realloc(bytes, 16)
    on alloc_size(grown) != 16 => return 5
    on grown.data[0] != 42 => return 7

    leak(grown)
    free(grown)

    return 42
}
¬
42
¬

¬
delete
¬
