main :: fn () -> i32 {
    empty: []u8 = nil
    text: [2]u8 = ['a', 'b']
    view := text[..]

    on empty != nil => return 1
    on nil != empty => return 2
    on view == nil => return 3
    on nil == view => return 4

    return 0
}
¬
0
¬

¬

¬
