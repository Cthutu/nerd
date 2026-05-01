Point :: plex {
    x i32
    y i32
    z i32
}

main :: fn () -> i32 {
    p := Point { x: 7, ... }
    return p.x + p.y + p.z
}
¬
7
¬
¬
fn main
$0 = plex(x: i32:7, y: i32:0, z: i32:0)
local p = plex{x:i32,y:i32,z:i32}:$0
$1 = plex{x:i32,y:i32,z:i32}:p.x
$2 = plex{x:i32,y:i32,z:i32}:p.y
$3 = i32:$1 + i32:$2
$4 = plex{x:i32,y:i32,z:i32}:p.z
$5 = i32:$3 + i32:$4
return i32:$5
end
¬
¬
