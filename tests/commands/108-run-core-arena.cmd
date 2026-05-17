use std.io
use core

main :: fn () -> i32 {
    a: Arena
    a = arena(16, 8)

    first := alloc[i32](^a)
    first^ = 41

    bytes := alloc_array[u8](^a, 9000)
    bytes[0] = 1
    bytes[8999] = 2

    first^ += 1
    prn($"first={first^} bytes={bytes[0]}:{bytes[8999]}")

    a.reset()
    reused := alloc[i32](^a)
    reused^ = 5

    result := on first == reused {
        yes  => reused^
        else => 99
    }

    a.done()
    return result
}
¬
5
¬
first=42 bytes=1:2

¬
delete
¬
--llvm
