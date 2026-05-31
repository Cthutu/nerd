Inner :: plex {
    value i32
}

Outer :: plex {
    inner Inner
}

write_value :: fn (value: ^i32, replacement: i32) {
    value^ = replacement
}

main :: fn () -> i32 {
    outer := Outer { inner: { value: 1 } }
    outer_ptr := ^outer
    write_value(^(outer_ptr.inner.value), 42)
    return outer.inner.value - 42
}
¬
0
¬

¬
