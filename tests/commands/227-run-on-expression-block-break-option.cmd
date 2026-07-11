make :: fn (ok: bool) -> ?i32 {
    on ok => {
        value := 42
        return value
    }
    return nil
}

main :: fn () -> i32 {
    result := make(yes)
    on result {
        value => {
            on (value != 42) => return 1
            return 0
        }
        else => return 2
    }
}
¬
0
¬

¬
delete
¬
