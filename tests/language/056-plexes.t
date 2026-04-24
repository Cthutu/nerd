Point :: plex {
    x i32
    y i32
    name string
}

main :: fn () -> i32 {
    p: Point = Point { x: 3, y: 4, name: "origin" }
    q := Point { name: "other", x: 5, y: 6 }
    prn($"p {p.name}: {p.x}, {p.y}")
    prn($"q {q.name}: {q.x}, {q.y}")
    return p.x + p.y + q.x + q.y
}

¬
18
¬
p origin: 3, 4
q other: 5, 6

¬
fn main
string.reset
$0 = plex(x: i32:3, y: i32:4, name: string:"origin")
local p = plex{x:i32,y:i32,name:string}:$0
$1 = plex(name: string:"other", x: i32:5, y: i32:6)
local q = plex{x:i32,y:i32,name:string}:$1
$2 = string.start
string.append string:"p "
$4 = plex{x:i32,y:i32,name:string}:p.name
string.append string:$4
string.append string:": "
$5 = plex{x:i32,y:i32,name:string}:p.x
string.append i32:$5
string.append string:", "
$6 = plex{x:i32,y:i32,name:string}:p.y
string.append i32:$6
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
$7 = string.start
string.append string:"q "
$9 = plex{x:i32,y:i32,name:string}:q.name
string.append string:$9
string.append string:": "
$10 = plex{x:i32,y:i32,name:string}:q.x
string.append i32:$10
string.append string:", "
$11 = plex{x:i32,y:i32,name:string}:q.y
string.append i32:$11
$8 = string.finish $7
call fn(string)->void:prn, string:$8
string.reset
$12 = plex{x:i32,y:i32,name:string}:p.x
$13 = plex{x:i32,y:i32,name:string}:p.y
$14 = i32:$12 + i32:$13
$15 = plex{x:i32,y:i32,name:string}:q.x
$16 = i32:$14 + i32:$15
$17 = plex{x:i32,y:i32,name:string}:q.y
$18 = i32:$16 + i32:$17
return i32:$18
end¬
void init() {}
typedef struct plex8 {
    int $x;
    int $y;
    string $name;
} plex8;
int $main() {
    string_builder_reset();
    plex8 $0 = (plex8){.$x = 3, .$y = 4, .$name = (string){.data = (u8*)"origin", .count = 6}};
    plex8 $p = $0;
    plex8 $1 = (plex8){.$name = (string){.data = (u8*)"other", .count = 5}, .$x = 5, .$y = 6};
    plex8 $q = $1;
    size_t $2 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"p ", .count = 2}));
    string $4 = $p.$name;
    string_builder_append_string(to_string$string($4));
    string_builder_append_string(to_string$string((string){.data = (u8*)": ", .count = 2}));
    int $5 = $p.$x;
    string_builder_append_string(to_string$i32($5));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    int $6 = $p.$y;
    string_builder_append_string(to_string$i32($6));
    string $3 = string_builder_finish($2);
    prn($3);
    string_builder_reset();
    size_t $7 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"q ", .count = 2}));
    string $9 = $q.$name;
    string_builder_append_string(to_string$string($9));
    string_builder_append_string(to_string$string((string){.data = (u8*)": ", .count = 2}));
    int $10 = $q.$x;
    string_builder_append_string(to_string$i32($10));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    int $11 = $q.$y;
    string_builder_append_string(to_string$i32($11));
    string $8 = string_builder_finish($7);
    prn($8);
    string_builder_reset();
    int $12 = $p.$x;
    int $13 = $p.$y;
    int $14 = $12 + $13;
    int $15 = $q.$x;
    int $16 = $14 + $15;
    int $17 = $q.$y;
    int $18 = $16 + $17;
    return $18;
}
