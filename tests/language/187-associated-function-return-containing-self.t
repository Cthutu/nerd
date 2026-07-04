Box :: plex {
    value i32
}

impl Box {
    try_new :: fn (value: i32) -> Option[Self] {
        return Some(Self { value })
    }
}

main :: fn () -> i32 {
    box := on Box.try_new(42) {
        Some(value) => value
        None        => return 1
    }
    return box.value
}
¬
42
¬

¬

¬
