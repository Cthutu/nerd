Payload :: plex {
    width  u32
    height u32
}

main :: fn () -> i32 {
    value : Payload\string = { width: 1 height: 2 }

    on value => [header] {
        on header.width == 0 || header.height == 0 => return 1
        return 0
    } else {
        return 2
    }
}
¬
0
¬

¬
delete
