use std.print

Colour :: enum { Red Green Blue }

score :: fn (colour: Colour) -> i32 {
    return on colour {
        Red => 10
        Green => 20
        Blue => 30
    }
}

pick_shadowed :: fn () -> Colour {
    Red: Colour = Green
    picked: Colour = Red
    return picked
}

main :: fn () -> i32 {
    red: Colour = Red
    green: Colour = Green
    blue := Colour.Blue

    prn($"red {score(red)}")
    prn($"green {score(green)}")
    prn($"blue {score(blue)}")
    prn($"shadowed {score(pick_shadowed())}")
    prn($"total {score(red) + score(green) + score(blue)}")

    return 0
}
¬
0
¬
red 10
green 20
blue 30
shadowed 10
total 60

¬
fn score
param enum{Red,Green,Blue}:colour
local $0 = i32:0
$4 = enum(0)
$5 = enum{Red,Green,Blue}:colour == enum{Red,Green,Blue}:$4
branch.false bool:$5, L2
label L3
$0 = i32:10
jump L1
label L2
$8 = enum(1)
$9 = enum{Red,Green,Blue}:colour == enum{Red,Green,Blue}:$8
branch.false bool:$9, L6
label L7
$0 = i32:20
jump L1
label L6
$11 = enum(2)
$12 = enum{Red,Green,Blue}:colour == enum{Red,Green,Blue}:$11
branch.false bool:$12, L1
label L10
$0 = i32:30
label L1
return i32:$0
end
fn pick_shadowed
$0 = enum(1)
local Red = enum{Red,Green,Blue}:$0
$1 = enum(0)
local picked = enum{Red,Green,Blue}:$1
return enum{Red,Green,Blue}:picked
end
fn main
string.reset
$0 = enum(0)
local red = enum{Red,Green,Blue}:$0
$1 = enum(1)
local green = enum{Red,Green,Blue}:$1
$2 = enum(2)
local blue = enum{Red,Green,Blue}:$2
$3 = string.start
string.append string:"red "
$5 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:red
string.append i32:$5
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$6 = string.start
string.append string:"green "
$8 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:green
string.append i32:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$9 = string.start
string.append string:"blue "
$11 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:blue
string.append i32:$11
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
$12 = string.start
string.append string:"shadowed "
$14 = call fn()->enum{Red,Green,Blue}:pick_shadowed
$15 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:$14
string.append i32:$15
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$16 = string.start
string.append string:"total "
$18 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:red
$19 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:green
$20 = i32:$18 + i32:$19
$21 = call fn(enum{Red,Green,Blue})->i32:score, enum{Red,Green,Blue}:blue
$22 = i32:$20 + i32:$21
string.append i32:$22
$17 = string.finish $16
call fn(string)->void:prn, string:$17
string.reset
return i32:0
end
¬
void init() {}
typedef struct enum7 {
    uint8_t tag;
    union { uint8_t unit; } data;
} enum7;
int $score(enum7 $colour) {
    int $0 = 0;
    enum7 $4 = (enum7){.tag = 0};
    bool $5 = $colour.tag == $4.tag;
    if (!$5) goto L2;
    L3: ;
    $0 = 10;
    goto L1;
    L2: ;
    enum7 $8 = (enum7){.tag = 1};
    bool $9 = $colour.tag == $8.tag;
    if (!$9) goto L6;
    L7: ;
    $0 = 20;
    goto L1;
    L6: ;
    enum7 $11 = (enum7){.tag = 2};
    bool $12 = $colour.tag == $11.tag;
    if (!$12) goto L1;
    L10: ;
    $0 = 30;
    L1: ;
    return $0;
}
enum7 $pick_shadowed() {
    enum7 $0 = (enum7){.tag = 1};
    enum7 $Red = $0;
    enum7 $1 = (enum7){.tag = 0};
    enum7 $picked = $1;
    return $picked;
}
int $main() {
    string_builder_reset();
    enum7 $0 = (enum7){.tag = 0};
    enum7 $red = $0;
    enum7 $1 = (enum7){.tag = 1};
    enum7 $green = $1;
    enum7 $2 = (enum7){.tag = 2};
    enum7 $blue = $2;
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"red ", .count = 4}));
    int $5 = $score($red);
    string_builder_append_string(to_string$i32($5));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"green ", .count = 6}));
    int $8 = $score($green);
    string_builder_append_string(to_string$i32($8));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $9 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"blue ", .count = 5}));
    int $11 = $score($blue);
    string_builder_append_string(to_string$i32($11));
    string $10 = string_builder_finish($9);
    prn($10);
    string_builder_reset();
    size_t $12 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"shadowed ", .count = 9}));
    enum7 $14 = $pick_shadowed();
    int $15 = $score($14);
    string_builder_append_string(to_string$i32($15));
    string $13 = string_builder_finish($12);
    prn($13);
    string_builder_reset();
    size_t $16 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"total ", .count = 6}));
    int $18 = $score($red);
    int $19 = $score($green);
    int $20 = $18 + $19;
    int $21 = $score($blue);
    int $22 = $20 + $21;
    string_builder_append_string(to_string$i32($22));
    string $17 = string_builder_finish($16);
    prn($17);
    string_builder_reset();
    return 0;
}
