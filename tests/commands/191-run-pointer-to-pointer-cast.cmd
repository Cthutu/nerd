main :: fn () -> i32 {
    value : u8 = 65
    ptr := (^value).as(^i8)
    return ptr[0].as(i32) - 65
}
¬
0
¬

¬
delete
¬
¬
run
