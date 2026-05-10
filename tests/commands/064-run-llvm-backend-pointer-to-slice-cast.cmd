main :: fn () -> i32 {
    text: [4]u8 = ['a', 'b', 'c', 'd']
    ptr := text[..].data
    view := ptr.as([]u8, 2)
    empty: []u8 = nil

    return view.count.as(i32) * 100 + view[0].as(i32) + empty.count.as(i32)
}
¬
41
¬

¬
delete
¬
--llvm-backend
