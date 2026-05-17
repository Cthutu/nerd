-- test-platform: linux
use core

main :: fn () -> i32 {
    a := arena(16, 8)
    defer a.done()

    bytes := a.alloc_bytes(4_294_967_296)
    on bytes.count > 0 => return 2
    return 1
}
¬
127
¬

¬
delete
¬
--llvm
¬
run
