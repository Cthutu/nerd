main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    (a, b) = (b, a)
    prn($"{a} {b}")
    return 0
}

¬
0
¬
2 3
3 2

¬
fn main
string.reset
$0 = tuple(i32:2, i32:3)
$1 = (i32,i32):$0.0
local a = i32:$1
$2 = (i32,i32):$0.1
local b = i32:$2
$3 = string.start
string.append i32:a
string.append string:" "
string.append i32:b
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$5 = tuple(i32:b, i32:a)
$6 = (i32,i32):$5.0
a = i32:$6
$7 = (i32,i32):$5.1
b = i32:$7
$8 = string.start
string.append i32:a
string.append string:" "
string.append i32:b
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
return i32:0
end
¬
void init() {}
typedef struct tuple8 {
    int _0;
    int _1;
} tuple8;
int $main() {
    string_builder_reset();
    tuple8 $0 = (tuple8){._0 = 2, ._1 = 3};
    int $1 = $0._0;
    int $a = $1;
    int $2 = $0._1;
    int $b = $2;
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$i32($a));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($b));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    tuple8 $5 = (tuple8){._0 = $b, ._1 = $a};
    int $6 = $5._0;
    $a = $6;
    int $7 = $5._1;
    $b = $7;
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$i32($a));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($b));
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    return 0;
}
