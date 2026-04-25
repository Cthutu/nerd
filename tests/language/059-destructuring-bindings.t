main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    return 0
}

¬
0
¬
2 3

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
    return 0;
}
