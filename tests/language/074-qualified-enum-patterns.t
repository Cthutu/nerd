Maybe :: enum { None Some(i32) Pair(i32, i32) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        Maybe.None => 0
        Maybe.Some(as x) => x
        Maybe.Pair(as left, as right) => left + right
    }
}

main :: fn () -> i32 {
    return score(Maybe.Pair(10, 20))
}
¬
30
¬

¬
fn score
param enum{None,Some(i32),Pair((i32,i32))}:value
local $0 = i32:0
$4 = enum(0)
$5 = enum{None,Some(i32),Pair((i32,i32))}:value == enum{None,Some(i32),Pair((i32,i32))}:$4
branch.false bool:$5, L2
label L3
$0 = i32:0
jump L1
label L2
$8 = enum(1)
$9 = enum{None,Some(i32),Pair((i32,i32))}:value == enum{None,Some(i32),Pair((i32,i32))}:$8
branch.false bool:$9, L6
$10 = enum{None,Some(i32),Pair((i32,i32))}:value.payload(1)
$11 = enum{None,Some(i32),Pair((i32,i32))}:value.payload(1)
local score$x$on43 = i32:$11
label L7
$0 = i32:score$x$on43
jump L1
label L6
$13 = enum(2)
$14 = enum{None,Some(i32),Pair((i32,i32))}:value == enum{None,Some(i32),Pair((i32,i32))}:$13
branch.false bool:$14, L1
$15 = enum{None,Some(i32),Pair((i32,i32))}:value.payload(2)
$16 = (i32,i32):$15.0
$17 = (i32,i32):$15.1
$18 = enum{None,Some(i32),Pair((i32,i32))}:value.payload(2)
$19 = (i32,i32):$18.0
local score$left$on52 = i32:$19
$20 = (i32,i32):$18.1
local score$right$on55 = i32:$20
label L12
$21 = i32:score$left$on52 + i32:score$right$on55
$0 = i32:$21
label L1
return i32:$0
end
fn main
$0 = tuple(i32:10, i32:20)
$1 = enum(2) (i32,i32):$0
$2 = call fn(enum{None,Some(i32),Pair((i32,i32))})->i32:score, enum{None,Some(i32),Pair((i32,i32))}:$1
return i32:$2
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
    int $score$x$on43 = $11;
    L7: ;
    $0 = $score$x$on43;
    goto L1;
    L6: ;
    enum8 $13 = (enum8){.tag = 2};
    bool $14 = $value.tag == $13.tag;
    if (!$14) goto L1;
    tuple7 $15 = $value.data.$Pair;
    int $16 = $15._0;
    int $17 = $15._1;
    tuple7 $18 = $value.data.$Pair;
    int $19 = $18._0;
    int $score$left$on52 = $19;
    int $20 = $18._1;
    int $score$right$on55 = $20;
    L12: ;
    int $21 = $score$left$on52 + $score$right$on55;
    $0 = $21;
    L1: ;
    return $0;
}
int $main() {
    tuple7 $0 = (tuple7){._0 = 10, ._1 = 20};
    enum8 $1 = (enum8){.tag = 2, .data.$Pair = $0};
    int $2 = $score($1);
    return $2;
}
