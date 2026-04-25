Colour :: enum { RED GREEN BLUE }

main :: fn () {
    colour : Colour = .RED

    prn($"Colour = {on colour { .RED => "red" .GREEN => "green" .BLUE => "blue" }}")
}
¬
0
¬
Colour = red

¬
fn main
string.reset
$0 = enum(0)
local colour = enum{RED,GREEN,BLUE}:$0
$1 = string.start
string.append string:"Colour = "
local $3 = string:0
$7 = enum(0)
$8 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$7
branch.false bool:$8, L5
label L6
$3 = string:"red"
jump L4
label L5
$11 = enum(1)
$12 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$11
branch.false bool:$12, L9
label L10
$3 = string:"green"
jump L4
label L9
$14 = enum(2)
$15 = enum{RED,GREEN,BLUE}:colour == enum{RED,GREEN,BLUE}:$14
branch.false bool:$15, L4
label L13
$3 = string:"blue"
label L4
string.append string:$3
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
return i32:0
end
¬
void init() {}
typedef struct enum8 {
    uint8_t tag;
    union { uint8_t unit; } data;
} enum8;
int $main() {
    string_builder_reset();
    enum8 $0 = (enum8){.tag = 0};
    enum8 $colour = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"Colour = ", .count = 9}));
    string $3 = (string){0};
    enum8 $7 = (enum8){.tag = 0};
    bool $8 = $colour.tag == $7.tag;
    if (!$8) goto L5;
    L6: ;
    $3 = (string){.data = (u8*)"red", .count = 3};
    goto L4;
    L5: ;
    enum8 $11 = (enum8){.tag = 1};
    bool $12 = $colour.tag == $11.tag;
    if (!$12) goto L9;
    L10: ;
    $3 = (string){.data = (u8*)"green", .count = 5};
    goto L4;
    L9: ;
    enum8 $14 = (enum8){.tag = 2};
    bool $15 = $colour.tag == $14.tag;
    if (!$15) goto L4;
    L13: ;
    $3 = (string){.data = (u8*)"blue", .count = 4};
    L4: ;
    string_builder_append_string(to_string$string($3));
    string $2 = string_builder_finish($1);
    prn($2);
    string_builder_reset();
    return 0;
}
