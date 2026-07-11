Payload :: plex {
    width  u32
    height u32
}

main :: fn () -> i32 {
    value : Payload\string = { width: 1 height: 2 }

    header := on value => [parsed_header] {
        break parsed_header
    } else {
        return 1
    }

    return (header.width + header.height).as(i32) - 3
}
¬
0
¬

¬
delete
