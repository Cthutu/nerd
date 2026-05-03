io :: use std.io

id :: fn [T] (value: T) => value

choose :: fn [T] (left: T, right: T, use_left: bool) => on use_left {
    yes => left
    else => right
}

Box :: plex [T] {
    value T
}

wrap :: fn [T] (value: T) -> Box[T] {
    box: Box[T]
    box.value = value
    return box
}

main :: fn () {
    number := id(7)
    text := id("ok")
    explicit := id[i32](number + 5)

    int_id := id[i32]
    from_value := int_id(explicit + 30)

    box := wrap[string](text)
    chosen := choose[string](box.value, "bad", yes)

    io.prn($"{number} {text} {explicit} {from_value} {chosen}")
}
¬
0
¬
7 ok 12 42 ok

¬

¬
