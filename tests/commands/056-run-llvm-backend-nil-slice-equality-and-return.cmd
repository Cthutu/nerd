make :: fn (ok: bool) -> []u8 {
    on ok == no => return nil

    text: [2]u8 = ['o', 'k']
    return text[..]
}

main :: fn () -> i32 {
    empty: []u8 = nil
    text: [2]u8 = ['a', 'b']
    view := text[..]

    on empty != nil => return 1
    on nil != empty => return 2
    on view == nil => return 3
    on nil == view => return 4

    returned_empty := make(no)
    returned_full := make(yes)
    on returned_empty != nil => return 5
    on returned_full == nil => return 6

    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
