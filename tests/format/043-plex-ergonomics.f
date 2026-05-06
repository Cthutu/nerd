use std.io

Point::plex#c{x i32 y i32 name string}
Packed::plex#packed{a u8 b i32}

main::fn(){
p:=Point{x:x,y:y,name:name}
short:=Point{x,y,name}
q:=p with{
y
name:"second"
}
pp:=^q
prn($"{pp.name} {pp.x} {pp.y}")
packed:=Packed{a:1,b:2}
}
¬
use std.io

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
    p := Point { x, y, name }
    short := Point { x, y, name }
    q := p with {
        y
        name : "second"
    }
    pp := ^q
    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1, b: 2 }
}
