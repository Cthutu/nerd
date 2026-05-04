Thing :: plex {
    size (i32, i32)
}

main :: fn () {
    thing: Thing
    thing.size = (7, 5)
    return thing.size.0 + thing.size.1
}
¬
12
¬

¬

¬
