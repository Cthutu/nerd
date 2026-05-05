Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

impl Box[T] {
    init :: fn (value: T) -> Self {
        return { value }
    }
}

main :: fn () -> i32 {
    box := IntBox.init(42)
    return box.value
}
¬
42
¬
¬
¬
