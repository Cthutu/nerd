Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

global_box := IntBox.init(40)

impl Box[T] {
    init :: fn (value: T) -> Self {
        return { value }
    }
}

main :: fn () -> i32 {
    box := IntBox.init(2)
    return global_box.value + box.value
}
¬
42
¬
¬
¬
