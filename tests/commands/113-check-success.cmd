use core

main :: fn () -> i32 {
    arena := arena(4096)
    value := arena.mark()
    return on value == 0 {
        yes  => 0
        else => 1
    }
}
¬
0
¬

¬
delete
¬

¬
check
