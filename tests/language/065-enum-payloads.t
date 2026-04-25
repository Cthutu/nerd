use mod std.print

Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Some(x) => x
        Pair(left, right) => left + right
        Text(_) => 100
    }
}

main :: fn () -> i32 {
    a : Maybe = None
    b : Maybe = Some(5)
    c := Maybe.Pair(10, 20)
    d : Maybe = Text("hello")

    prn($"scores {score(a)} {score(b)} {score(c)} {score(d)}")

    return score(c)
}
¬
30
¬
scores 0 5 30 100

¬
fn score
param enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value
local $0 = i32:0
$4 = enum(0)
$5 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$4
branch.false bool:$5, L2
label L3
$0 = i32:0
jump L1
label L2
$8 = enum(1)
$9 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$8
branch.false bool:$9, L6
$10 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(1)
$11 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(1)
local score$x$on47 = i32:$11
label L7
$0 = i32:score$x$on47
jump L1
label L6
$14 = enum(2)
$15 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$14
branch.false bool:$15, L12
$16 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(2)
$17 = (i32,i32):$16.0
$18 = (i32,i32):$16.1
$19 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(2)
$20 = (i32,i32):$19.0
local score$left$on53 = i32:$20
$21 = (i32,i32):$19.1
local score$right$on55 = i32:$21
label L13
$22 = i32:score$left$on53 + i32:score$right$on55
$0 = i32:$22
jump L1
label L12
$24 = enum(3)
$25 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value == enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$24
branch.false bool:$25, L1
$26 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(3)
$27 = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:value.payload(3)
label L23
$0 = i32:100
label L1
return i32:$0
end
fn main
string.reset
$0 = enum(0)
local a = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$0
$1 = enum(1) i32:5
local b = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$1
$2 = tuple(i32:10, i32:20)
$3 = enum(2) (i32,i32):$2
local c = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$3
$4 = enum(3) string:"hello"
local d = enum{None,Some(i32),Pair((i32,i32)),Text(string)}:$4
$5 = string.start
string.append string:"scores "
$7 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:a
string.append i32:$7
string.append string:" "
$8 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:b
string.append i32:$8
string.append string:" "
$9 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:c
string.append i32:$9
string.append string:" "
$10 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:d
string.append i32:$10
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$11 = call fn(enum{None,Some(i32),Pair((i32,i32)),Text(string)})->i32:score, enum{None,Some(i32),Pair((i32,i32)),Text(string)}:c
return i32:$11
end
¬
void init() {}
typedef struct tuple7 {
    int _0;
    int _1;
} tuple7;
typedef struct enum8 {
    uint8_t tag;
    union {
        int $Some;
        tuple7 $Pair;
        string $Text;
    } data;
} enum8;
int $score(enum8 $value) {
    int $0 = 0;
    enum8 $4 = (enum8){.tag = 0};
    bool $5 = $value.tag == $4.tag;
    if (!$5) goto L2;
    L3: ;
    $0 = 0;
    goto L1;
    L2: ;
    enum8 $8 = (enum8){.tag = 1};
    bool $9 = $value.tag == $8.tag;
    if (!$9) goto L6;
    int $10 = $value.data.$Some;
    int $11 = $value.data.$Some;
    int $score$x$on47 = $11;
    L7: ;
    $0 = $score$x$on47;
    goto L1;
    L6: ;
    enum8 $14 = (enum8){.tag = 2};
    bool $15 = $value.tag == $14.tag;
    if (!$15) goto L12;
    tuple7 $16 = $value.data.$Pair;
    int $17 = $16._0;
    int $18 = $16._1;
    tuple7 $19 = $value.data.$Pair;
    int $20 = $19._0;
    int $score$left$on53 = $20;
    int $21 = $19._1;
    int $score$right$on55 = $21;
    L13: ;
    int $22 = $score$left$on53 + $score$right$on55;
    $0 = $22;
    goto L1;
    L12: ;
    enum8 $24 = (enum8){.tag = 3};
    bool $25 = $value.tag == $24.tag;
    if (!$25) goto L1;
    string $26 = $value.data.$Text;
    string $27 = $value.data.$Text;
    L23: ;
    $0 = 100;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    enum8 $0 = (enum8){.tag = 0};
    enum8 $a = $0;
    enum8 $1 = (enum8){.tag = 1, .data.$Some = 5};
    enum8 $b = $1;
    tuple7 $2 = (tuple7){._0 = 10, ._1 = 20};
    enum8 $3 = (enum8){.tag = 2, .data.$Pair = $2};
    enum8 $c = $3;
    enum8 $4 = (enum8){.tag = 3, .data.$Text = (string){.data = (u8*)"hello", .count = 5}};
    enum8 $d = $4;
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"scores ", .count = 7}));
    int $7 = $score($a);
    string_builder_append_string(to_string$i32($7));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $8 = $score($b);
    string_builder_append_string(to_string$i32($8));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $9 = $score($c);
    string_builder_append_string(to_string$i32($9));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $10 = $score($d);
    string_builder_append_string(to_string$i32($10));
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    int $11 = $score($c);
    return $11;
}
