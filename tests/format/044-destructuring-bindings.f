use std.print

Point::plex{x i32 y i32 name string}

main::fn(){
(a,b):=(2,3)
(a,b)=(b,a)
(c,d)::(4,"four")
(e,f):(i32,string)=(5,"five")
{x,y,name}:=Point{x:5,y:6,name:"pt"}
prn($"{a} {b} {name} {x} {y}")
}
¬
use std.print

Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () {
    (a, b) := (2, 3)
    (a, b) = (b, a)
    (c, d) :: (4, "four")
    (e, f): (i32, string) = (5, "five")
    { x, y, name } := Point { x: 5, y: 6, name: "pt" }
    prn($"{a} {b} {name} {x} {y}")
}
