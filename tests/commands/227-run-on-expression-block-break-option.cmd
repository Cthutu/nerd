make :: fn (ok: bool) -> Option[i32] {
    return on ok => ${
        value := 42
        break Some(value)
    } else None
}

main :: fn () -> i32 {
    result := make(yes)
    on result {
        Some(value) => {
            on (value != 42) => return 1
            return 0
        }
        None => return 2
    }
}
¬
0
¬

¬
delete
¬
