Thing :: plex {
    value i32
}

impl Thing {
    touch :: fn (thing: ^Self, _value: i32) -> bool {
        return no
    }
}

main :: fn () -> i32 {
    thing := Thing { value: 1 }
    _ := thing.touch(2)
    return thing.value
}
¬
1
¬

¬

¬
