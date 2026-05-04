-- Uses shorthand plex fields where `name` means `name: name`.
Point :: plex {
    x i32
    y i32
}

main :: fn () -> i32 {
    x := 7
    y := 11
    point := Point { x, y }
    other := point with { x }
    return point.x + point.y + other.x
}
¬
25
¬
¬
fn main
local x = i32:7
local y = i32:11
$0 = plex(x: i32:x, y: i32:y)
local point = plex{x:i32,y:i32}:$0
$1 = plex{x:i32,y:i32}:point.y
$2 = plex(x: i32:x, y: i32:$1)
local other = plex{x:i32,y:i32}:$2
$3 = plex{x:i32,y:i32}:point.x
$4 = plex{x:i32,y:i32}:point.y
$5 = i32:$3 + i32:$4
$6 = plex{x:i32,y:i32}:other.x
$7 = i32:$5 + i32:$6
return i32:$7
end
¬
void init() {}
#ifndef NERD_TYPE_plexfe8b8667
#define NERD_TYPE_plexfe8b8667
typedef struct plexfe8b8667 {
    int $x;
    int $y;
} plexfe8b8667;
#endif
int $main() {
    int $x = 7;
    int $y = 11;
    plexfe8b8667 $0 = (plexfe8b8667){.$x = $x, .$y = $y};
    plexfe8b8667 $point = $0;
    int $1 = $point.$y;
    plexfe8b8667 $2 = (plexfe8b8667){.$x = $x, .$y = $1};
    plexfe8b8667 $other = $2;
    int $3 = $point.$x;
    int $4 = $point.$y;
    int $5 = $3 + $4;
    int $6 = $other.$x;
    int $7 = $5 + $6;
    return $7;
}
