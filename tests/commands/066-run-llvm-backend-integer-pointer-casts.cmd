main :: fn () -> i32 {
    null_ptr: ^u8 = 0
    display_addr :: 0x1000
    display: ^void = display_addr
    other := 0x2000.as(^u8)

    on null_ptr != nil => return 1
    on display == nil => return 2
    on other == nil => return 3
    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
