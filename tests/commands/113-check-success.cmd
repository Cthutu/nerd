

main :: fn () -> i32 {
    scratch := arena(4096)
    value := scratch.mark()
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
