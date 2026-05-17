use std.io
use core

main :: fn () -> i32 {
    a := arena(16, 8)
    defer a.done()

    first := a.alloc[i32]()
    first^ = 11

    mark := a.mark()
    second := a.alloc[i32]()
    second^ = 22

    bytes := a.alloc_array[u8](9000)
    bytes[0] = 3
    bytes[8999] = 4

    on first^ != 11 => return 2

    a.restore(mark)
    replacement := a.alloc[i32]()
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
