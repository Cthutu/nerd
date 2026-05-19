main :: fn () -> i32 {
    value : u8 = 7
    ptr := ^value
    addr := ptr.as(usize)

    on addr == 0 => return 1
    return 0
}
¬
0
¬

¬
delete
¬
