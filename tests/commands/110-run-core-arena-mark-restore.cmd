use std.io
use core

main :: fn () -> i32 {
    a := arena(16, 8)
    defer a.done()

    base := a.base
    first := alloc[i32](^a)
    first^ = 11

    mark := a.mark()
    second := alloc[i32](^a)
    second^ = 22

    bytes := alloc_array[u8](^a, 9000)
    bytes[0] = 3
    bytes[8999] = 4

    on a.base != base => return 1
    on first^ != 11 => return 2

    a.restore(mark)
    replacement := alloc[i32](^a)
    replacement^ = 33

    on replacement != second => return 3
    prn($"first={first^} replacement={replacement^}")
    return first^ + replacement^
}
¬
44
¬
first=11 replacement=33

¬
delete
¬
--llvm
