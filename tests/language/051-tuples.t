use mod std.print

make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

main :: fn () {
    pair :: (7, "seven")
    single :: (pair.0 + 1,)
    from_fn :: make_pair(3, "three")
    nested :: (pair, single, yes)

    prn($"pair = {pair.0}, {pair.1}")
    prn($"pair tuple = {pair}")
    prn($"single = {single.0}")
    prn($"single tuple = {single}")
    prn($"from_fn = {from_fn.0}, {from_fn.1}")
    prn($"nested tuple = {nested}")

    return pair.0 + single.0 + from_fn.0
}
¬
18
¬
pair = 7, seven
pair tuple = (7, seven)
single = 8
single tuple = (8,)
from_fn = 3, three
nested tuple = ((7, seven), (8,), yes)

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
string.append string:"pair tuple = "
string.append (i32,string):$2
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$7 = string.start
string.append string:"single = "
$9 = (i32,string):$2.0
$10 = i32:$9 + i32:1
$11 = tuple(i32:$10)
$12 = (i32,):$11.0
string.append i32:$12
$8 = string.finish $7
call fn(string)->void:prn, string:$8
string.reset
$13 = string.start
string.append string:"single tuple = "
string.append (i32,):$11
$14 = string.finish $13
call fn(string)->void:prn, string:$14
string.reset
$15 = string.start
string.append string:"from_fn = "
$17 = call fn(i32,string)->(i32,string):make_pair, i32:3, string:"three"
$18 = (i32,string):$17.0
string.append i32:$18
string.append string:", "
$19 = (i32,string):$17.1
string.append string:$19
$16 = string.finish $15
call fn(string)->void:prn, string:$16
string.reset
$20 = string.start
string.append string:"nested tuple = "
$22 = tuple((i32,string):$2, (i32,):$11, bool:yes)
string.append ((i32,string),(i32,),bool):$22
$21 = string.finish $20
call fn(string)->void:prn, string:$21
string.reset
$23 = (i32,string):$2.0
$24 = (i32,):$11.0
$25 = i32:$23 + i32:$24
$26 = (i32,string):$17.0
$27 = i32:$25 + i32:$26
return i32:$27
end
¬
void init() {}
typedef struct tuple9 {
    int _0;
    string _1;
} tuple9;
typedef struct tuple11 {
    int _0;
} tuple11;
typedef struct tuple12 {
    tuple9 _0;
    tuple11 _1;
    bool _2;
} tuple12;
tuple9 $make_pair(int $a, string $b) {
    tuple9 $0 = (tuple9){._0 = $a, ._1 = $b};
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"pair = ", .count = 7}));
    tuple9 $2 = (tuple9){._0 = 7, ._1 = (string){.data = (u8*)"seven", .count = 5}};
    int $3 = $2._0;
    string_builder_append_string(to_string$i32($3));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    string $4 = $2._1;
    string_builder_append_string(to_string$string($4));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"pair tuple = ", .count = 13}));
    string_builder_append_string((string){.data = (u8*)"(", .count = 1});
    string_builder_append_string(to_string$i32($2._0));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$string($2._1));
    string_builder_append_string((string){.data = (u8*)")", .count = 1});
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    size_t $7 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"single = ", .count = 9}));
    int $9 = $2._0;
    int $10 = $9 + 1;
    tuple11 $11 = (tuple11){._0 = $10};
    int $12 = $11._0;
    string_builder_append_string(to_string$i32($12));
    string $8 = string_builder_finish($7);
    prn($8);
    string_builder_reset();
    size_t $13 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"single tuple = ", .count = 15}));
    string_builder_append_string((string){.data = (u8*)"(", .count = 1});
    string_builder_append_string(to_string$i32($11._0));
    string_builder_append_string((string){.data = (u8*)",", .count = 1});
    string_builder_append_string((string){.data = (u8*)")", .count = 1});
    string $14 = string_builder_finish($13);
    prn($14);
    string_builder_reset();
    size_t $15 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"from_fn = ", .count = 10}));
    tuple9 $17 = $make_pair(3, (string){.data = (u8*)"three", .count = 5});
    int $18 = $17._0;
    string_builder_append_string(to_string$i32($18));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    string $19 = $17._1;
    string_builder_append_string(to_string$string($19));
    string $16 = string_builder_finish($15);
    prn($16);
    string_builder_reset();
    size_t $20 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"nested tuple = ", .count = 15}));
    tuple12 $22 = (tuple12){._0 = $2, ._1 = $11, ._2 = true};
    string_builder_append_string((string){.data = (u8*)"(", .count = 1});
    string_builder_append_string((string){.data = (u8*)"(", .count = 1});
    string_builder_append_string(to_string$i32($22._0._0));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$string($22._0._1));
    string_builder_append_string((string){.data = (u8*)")", .count = 1});
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string((string){.data = (u8*)"(", .count = 1});
    string_builder_append_string(to_string$i32($22._1._0));
    string_builder_append_string((string){.data = (u8*)",", .count = 1});
    string_builder_append_string((string){.data = (u8*)")", .count = 1});
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$bool($22._2));
    string_builder_append_string((string){.data = (u8*)")", .count = 1});
    string $21 = string_builder_finish($20);
    prn($21);
    string_builder_reset();
    int $23 = $2._0;
    int $24 = $11._0;
    int $25 = $23 + $24;
    int $26 = $17._0;
    int $27 = $25 + $26;
    return $27;
}
