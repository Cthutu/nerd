use std.io

Point::plex{x i32 y i32 name string}

main::fn(){
Local::plex{value i32 label string}
p:Point=Point{x:1,y:2,name:"p"}
prn($"{p.name} {p.x} {p.y}")
}
¬
use std.io

Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () {
    Local :: plex {
        value i32
        label string
    }
    p : Point = Point { x: 1, y: 2, name: "p" }
    prn($"{p.name} {p.x} {p.y}")
}
