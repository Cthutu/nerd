Payload :: plex {
    width  u32
    height u32
}

main :: fn () -> i32 {
    value : Result[Payload, string] = Ok({ width: 1 height: 2 })

    header := on value {
        Ok(parsed_header) => parsed_header
        Err(_) => return 1
    }

    return (header.width + header.height).as(i32) - 3
}
¬
0
¬

¬
delete
