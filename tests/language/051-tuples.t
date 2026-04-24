make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

main :: fn () {
    pair :: (7, "seven")
    single :: (pair.0 + 1,)
    from_fn :: make_pair(3, "three")

    prn($"pair = {pair.0}, {pair.1}")
    prn($"single = {single.0}")
    prn($"from_fn = {from_fn.0}, {from_fn.1}")

    return pair.0 + single.0 + from_fn.0
}
¬
18
¬
pair = 7, seven
single = 8
from_fn = 3, three

¬
fn make_pair
param i32:a
param string:b
$0 = tuple(i32:a, string:b)
return (i32,string):$0
end
fn main
string.reset
$0 = string.start
string.append string:"pair = "
$2 = tuple(i32:7, string:"seven")
$3 = (i32,string):$2.0
string.append i32:$3
string.append string:", "
$4 = (i32,string):$2.1
string.append string:$4
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$5 = string.start
string.append string:"single = "
$7 = (i32,string):$2.0
$8 = i32:$7 + i32:1
$9 = tuple(i32:$8)
$10 = (i32,):$9.0
string.append i32:$10
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$11 = string.start
string.append string:"from_fn = "
$13 = call fn(i32,string)->(i32,string):make_pair, i32:3, string:"three"
$14 = (i32,string):$13.0
string.append i32:$14
string.append string:", "
$15 = (i32,string):$13.1
string.append string:$15
$12 = string.finish $11
call fn(string)->void:prn, string:$12
string.reset
$16 = (i32,string):$2.0
$17 = (i32,):$9.0
$18 = i32:$16 + i32:$17
$19 = (i32,string):$13.0
$20 = i32:$18 + i32:$19
return i32:$20
end
¬
void init() {}
typedef struct tuple8 {
    int _0;
    string _1;
} tuple8;
typedef struct tuple10 {
    int _0;
} tuple10;
tuple8 $make_pair(int $a, string $b) {
    tuple8 $0 = (tuple8){._0 = $a, ._1 = $b};
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"pair = ", .count = 7}));
    tuple8 $2 = (tuple8){._0 = 7, ._1 = (string){.data = (u8*)"seven", .count = 5}};
    int $3 = $2._0;
    string_builder_append_string(to_string$i32($3));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    string $4 = $2._1;
    string_builder_append_string(to_string$string($4));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"single = ", .count = 9}));
    int $7 = $2._0;
    int $8 = $7 + 1;
    tuple10 $9 = (tuple10){._0 = $8};
    int $10 = $9._0;
    string_builder_append_string(to_string$i32($10));
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    size_t $11 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"from_fn = ", .count = 10}));
    tuple8 $13 = $make_pair(3, (string){.data = (u8*)"three", .count = 5});
    int $14 = $13._0;
    string_builder_append_string(to_string$i32($14));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    string $15 = $13._1;
    string_builder_append_string(to_string$string($15));
    string $12 = string_builder_finish($11);
    prn($12);
    string_builder_reset();
    int $16 = $2._0;
    int $17 = $9._0;
    int $18 = $16 + $17;
    int $19 = $13._0;
    int $20 = $18 + $19;
    return $20;
}
