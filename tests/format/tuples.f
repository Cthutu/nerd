use std.io

Pair :: (i32,string)
Single :: (i32,)

main :: fn(){
pair :: (7,"seven")
single :: (pair.0+1,)
grouped :: (pair.0)
from_fn: (i32, string): (3, "three")
prn($"{pair.0} {pair.1} {single.0} {grouped} {from_fn.1}")
}
¬
use std.io

Pair   :: (i32, string)
Single :: (i32,)

main :: fn () {
    pair                    :: (7, "seven")
    single                  :: (pair.0 + 1,)
    grouped                 :: (pair.0)
    from_fn : (i32, string) : (3, "three")

    prn($"{pair.0} {pair.1} {single.0} {grouped} {from_fn.1}")
}
¬
use std.io

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
use std.io

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
    { x, y, name } := Point { x: 5 y: 6 name: "pt" }
    prn($"{a} {b} {name} {x} {y}")
}
