Payload :: plex {
    width  u32
    height u32
}

Thing :: enum {
    Done(Payload)
    Other
}

main :: fn () -> i32 {
    value : Thing = Thing.Done({ width: 1 height: 2 })

    on value {
        Thing.Done(header) => {
            on header.width == 0 || header.height == 0 => return 1
            return 0
        }
        Thing.Other => return 2
    }
}
¬
0
¬

¬
delete
