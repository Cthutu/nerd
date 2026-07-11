Box :: plex {
    value i32
}

impl Box {
    try_new :: fn (value: i32) -> ?Self {
        return Self { value }
    }
}

main :: fn () -> i32 {
    box := on Box.try_new(42) {
        value => value
        else => return 1
    }
    return box.value
}
¬
42
¬

¬

¬
