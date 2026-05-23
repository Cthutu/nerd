Map :: plex {
    width  u32
    height u32
}

Size :: plex {
    width  u32
    height u32
}

score :: fn (map: ^Map, size: Size) -> i32 {
return on map^ {{width: for size.width,height: for size.height}=>1 else=>2}
}
¬
Map :: plex {
    width  u32
    height u32
}

Size :: plex {
    width  u32
    height u32
}

score :: fn (map: ^Map, size: Size) -> i32 {
    return on map^ {
        { width: for size.width, height: for size.height } => 1
        else => 2
    }
}
