Map :: plex {
    value i32
}

global_map: box[Map]

take :: fn (map: box[Map]) -> i32 {
    on !map => return 1
    return map.value
}

make :: fn () -> box[Map] {
    map := box[Map]()
    map.value = 42
    return map
}

main :: fn () -> i32 {
    first := make()
    on !first => return 2

    result := take(first)
    on result != 42 => return 3
    on first => return 4

    second := make()
    third := second
    on second => return 5
    on !third => return 6

    third.free()
    on third => return 7

    global_map = make()
    on !global_map => return 8
    on take(global_map) != 42 => return 9
    on global_map => return 10

    return 42
}
¬
42
¬

¬
delete
¬
