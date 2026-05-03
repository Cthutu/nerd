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

main :: fn () -> i32 {
    p := Point { x: 1, y: 2, name: "first" }
    q := p with { y: 7, name: "second" }
    pp := ^q
    prn($"{p.name} {p.x} {p.y}")
    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1, b: 2 }
    return q.x + q.y + packed.b
}
¬
10
¬
first 1 2
second 1 7

¬
fn main
string.reset
$0 = plex(x: i32:1, y: i32:2, name: string:"first")
local p = plex{x:i32,y:i32,name:string}:$0
$1 = plex{x:i32,y:i32,name:string}:p.x
$2 = plex(x: i32:$1, y: i32:7, name: string:"second")
local q = plex{x:i32,y:i32,name:string}:$2
$3 = ^plex{x:i32,y:i32,name:string}:q
local pp = ^plex{x:i32,y:i32,name:string}:$3
$4 = string.start
$6 = plex{x:i32,y:i32,name:string}:p.name
string.append string:$6
string.append string:" "
$7 = plex{x:i32,y:i32,name:string}:p.x
string.append i32:$7
string.append string:" "
$8 = plex{x:i32,y:i32,name:string}:p.y
string.append i32:$8
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$9 = string.start
$11 = ^plex{x:i32,y:i32,name:string}:pp.name
string.append string:$11
string.append string:" "
$12 = ^plex{x:i32,y:i32,name:string}:pp.x
string.append i32:$12
string.append string:" "
$13 = ^plex{x:i32,y:i32,name:string}:pp.y
string.append i32:$13
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
$14 = plex(a: u8:1, b: i32:2)
local packed = plex{a:u8,b:i32}:$14
$15 = plex{x:i32,y:i32,name:string}:q.x
$16 = plex{x:i32,y:i32,name:string}:q.y
$17 = i32:$15 + i32:$16
$18 = plex{a:u8,b:i32}:packed.b
$19 = i32:$17 + i32:$18
return i32:$19
end
¬
void init() {}
#ifndef NERD_TYPE_plexa7899fed
#define NERD_TYPE_plexa7899fed
typedef struct plexa7899fed {
    int $x;
    int $y;
    string $name;
} plexa7899fed;
#endif
#ifndef NERD_TYPE_plex1ee601f8
#define NERD_TYPE_plex1ee601f8
typedef struct __attribute__((packed)) plex1ee601f8 {
    uint8_t $a;
    int $b;
} plex1ee601f8;
#endif
int $main() {
    string_builder_reset();
    plexa7899fed $0 = (plexa7899fed){.$x = 1, .$y = 2, .$name = (string){.data = (u8*)"first", .count = 5}};
    plexa7899fed $p = $0;
    int $1 = $p.$x;
    plexa7899fed $2 = (plexa7899fed){.$x = $1, .$y = 7, .$name = (string){.data = (u8*)"second", .count = 6}};
    plexa7899fed $q = $2;
    struct plexa7899fed* $3 = &$q;
    struct plexa7899fed* $pp = $3;
    size_t $4 = string_builder_mark();
    string $6 = $p.$name;
    string_builder_append_string(to_string$string($6));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $7 = $p.$x;
    string_builder_append_string(to_string$i32($7));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $8 = $p.$y;
    string_builder_append_string(to_string$i32($8));
    string $5 = string_builder_finish($4);
    prn($5);
    string_builder_reset();
    size_t $9 = string_builder_mark();
    string $11 = $pp->$name;
    string_builder_append_string(to_string$string($11));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $12 = $pp->$x;
    string_builder_append_string(to_string$i32($12));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $13 = $pp->$y;
    string_builder_append_string(to_string$i32($13));
    string $10 = string_builder_finish($9);
    prn($10);
    string_builder_reset();
    plex1ee601f8 $14 = (plex1ee601f8){.$a = 1, .$b = 2};
    plex1ee601f8 $packed = $14;
    int $15 = $q.$x;
    int $16 = $q.$y;
    int $17 = $15 + $16;
    int $18 = $packed.$b;
    int $19 = $17 + $18;
    return $19;
}
