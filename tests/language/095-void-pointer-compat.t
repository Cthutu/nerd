accept_void :: fn (ptr: ^void) -> i32 {
    view := ptr.as([]u8, 1)
    return view[0].as(i32) - 7
}

main :: fn () -> i32 {
    value: u8 = 7
    ptr := ^value
    return accept_void(ptr)
}
¬
0
¬

¬

¬
