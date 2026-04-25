use mod std.print

main :: fn () -> i32 {
    (a, b) := (2, 3)
    prn($"{a} {b}")
    (a, b) = (b, a)
    prn($"{a} {b}")
    (a, _) = (7, 8)
    prn($"{a} {b}")
    (c, d) :: (4, "four")
    prn($"{c} {d}")
    (e, f): (i32, string) = (5, "five")
    prn($"{e} {f}")
    return 0
}
¬
0
¬
2 3
3 2
7 2
4 four
5 five

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
$10 = tuple(i32:7, i32:8)
$11 = (i32,i32):$10.0
a = i32:$11
$12 = (i32,i32):$10.1
$13 = string.start
string.append i32:a
string.append string:" "
string.append i32:b
$14 = string.finish $13
call fn(string)->void:prn, string:$14
string.reset
$15 = tuple(i32:4, string:"four")
$16 = (i32,string):$15.0
local c = i32:$16
$17 = (i32,string):$15.1
local d = string:$17
$18 = string.start
string.append i32:c
string.append string:" "
string.append string:d
$19 = string.finish $18
call fn(string)->void:prn, string:$19
string.reset
$20 = tuple(i32:5, string:"five")
$21 = (i32,string):$20.0
local e = i32:$21
$22 = (i32,string):$20.1
local f = string:$22
$23 = string.start
string.append i32:e
string.append string:" "
string.append string:f
$24 = string.finish $23
call fn(string)->void:prn, string:$24
string.reset
return i32:0
end
¬
void init() {}
typedef struct tuple9 {
    int _0;
    int _1;
} tuple9;
typedef struct tuple10 {
    int _0;
    string _1;
} tuple10;
int $main() {
    string_builder_reset();
    tuple9 $0 = (tuple9){._0 = 2, ._1 = 3};
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
    tuple9 $5 = (tuple9){._0 = $b, ._1 = $a};
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
    tuple9 $10 = (tuple9){._0 = 7, ._1 = 8};
    int $11 = $10._0;
    $a = $11;
    int $12 = $10._1;
    size_t $13 = string_builder_mark();
    string_builder_append_string(to_string$i32($a));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($b));
    string $14 = string_builder_finish($13);
    prn($14);
    string_builder_reset();
    tuple10 $15 = (tuple10){._0 = 4, ._1 = (string){.data = (u8*)"four", .count = 4}};
    int $16 = $15._0;
    int $c = $16;
    string $17 = $15._1;
    string $d = $17;
    size_t $18 = string_builder_mark();
    string_builder_append_string(to_string$i32($c));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$string($d));
    string $19 = string_builder_finish($18);
    prn($19);
    string_builder_reset();
    tuple10 $20 = (tuple10){._0 = 5, ._1 = (string){.data = (u8*)"five", .count = 4}};
    int $21 = $20._0;
    int $e = $21;
    string $22 = $20._1;
    string $f = $22;
    size_t $23 = string_builder_mark();
    string_builder_append_string(to_string$i32($e));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$string($f));
    string $24 = string_builder_finish($23);
    prn($24);
    string_builder_reset();
    return 0;
}
