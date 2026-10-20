Map :: plex {
    x i32
}

main :: fn () {
    map : box[Map] = nil
    map = box[Map]()
    map.free()
}
¬
Map :: plex {
    x i32
}

main :: fn () {
    map : box[Map] = nil
    map = box[Map]()
    map.free()
}
