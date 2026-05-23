use std.io

Map :: plex {
    width u32
    height u32
}

Size :: plex {
    width u32
    height u32
}

score :: fn (map: ^Map, size: Size) -> i32 {
    return on map^ {
        { width: for size.width, height: for size.height } => 1
        else => 2
    }
}

main :: fn () -> i32 {
    map := Map { width: 80, height: 25 }
    same := Size { width: 80, height: 25 }
    different := Size { width: 100, height: 25 }

    a := score(^map, same)
    b := score(^map, different)
    prn($"postfix on {a} {b}")
    return a + b
}
¬
3
¬
postfix on 1 2

¬

¬
