Point::plex{x i32 y i32 name string}

main::fn(){
(a,b):=(2,3)
{x,y,name}:=Point{x:5,y:6,name:"pt"}
prn($"{a} {b} {name} {x} {y}")
}
¬
Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () {
    (a, b) := (2, 3)
    { x, y, name } := Point { x: 5, y: 6, name: "pt" }
    prn($"{a} {b} {name} {x} {y}")
}
