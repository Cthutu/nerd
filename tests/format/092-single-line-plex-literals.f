TermRect :: plex {
    x      i32
    y      i32
    width  u32
    height u32
}

main :: fn () {
    rect := TermRect { x: 10, y: 5, width: 20, height: 10 }
}
¬
TermRect :: plex {
    x      i32
    y      i32
    width  u32
    height u32
}

main :: fn () {
    rect := TermRect { x: 10, y: 5, width: 20, height: 10 }
}
