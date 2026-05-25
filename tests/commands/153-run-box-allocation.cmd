Map :: plex {
    x i32
}

read_map :: fn (map: ^Map) -> i32 {
    return map.x
}

main :: fn () -> i32 {
    map: box[Map] = nil
    on map != nil => return 1

    map = box[Map]()
    map.x = 41
    on read_map(map) != 41 => return 2

    map.free()
    on map != nil => return 3

    return 42
}
¬
42
¬

¬
delete
¬
