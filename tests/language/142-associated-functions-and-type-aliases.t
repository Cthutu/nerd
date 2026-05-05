Box :: plex [T] {
    value T
}

IntBox :: Box[i32]

global_box := IntBox.init(40)

impl Box[T] {
    init :: fn (value: T) -> Self {
        return { value }
    }

    item_size :: fn (self: Self) -> usize {
        return T.size
    }
}

main :: fn () -> i32 {
    box := IntBox.init(2)
    return global_box.value + box.value + box.item_size().as(i32) - i32.size.as(i32)
}
¬
42
¬
¬
¬
