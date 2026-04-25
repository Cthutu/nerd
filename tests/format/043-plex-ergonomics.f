use std.print

Point::plex#c{x i32 y i32 name string}
Packed::plex#packed{a u8 b i32}

main::fn(){
p:=Point{x:1,y:2,name:"first"}
q:=p with{
y:7
name:"second"
}
pp:=^q
prn($"{pp.name} {pp.x} {pp.y}")
packed:=Packed{a:1,b:2}
}
¬
use std.print

Point :: plex #c {
    x    i32
    y    i32
    name string
}

Packed :: plex #packed {
    a u8
    b i32
}

main :: fn () {
    p  :  = Point { x: 1, y: 2, name: "first" }
    q  :  = p with { y: 7, name: "second" }
    pp :  = ^q

    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1, b: 2 }
}
