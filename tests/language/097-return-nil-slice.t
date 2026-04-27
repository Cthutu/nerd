make :: fn (ok: bool) -> []u8 {
    on ok == no => return nil

    text: [2]u8 = ['o', 'k']
    return text[..]
}

main :: fn () -> i32 {
    empty := make(no)
    full := make(yes)

    on empty != nil => return 1
    on full == nil => return 2
    return 0
}
¬
0
¬

¬

¬
