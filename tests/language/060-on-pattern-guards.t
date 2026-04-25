score :: fn (size: u32) -> u32 {
    return on size {
        matched @ 0..3 on matched == 2 => matched + 20
        0..3 => 10
        exact @ _ on exact == 10 => exact * 10
        other @ else => other + 100
    }
}

main :: fn () {
    prn($"1: {score(1)}")
    prn($"2: {score(2)}")
    prn($"10: {score(10)}")
    prn($"50: {score(50)}")
}
¬
0
¬
1: 10
2: 22
10: 100
50: 150

¬
fn score
param u32:size
local $0 = u32:0
$4 = u32:0 <= u32:size
branch.false bool:$4, L2
$5 = u32:size < u32:3
branch.false bool:$5, L2
label L3
$6 = u32:size == u32:2
branch.false bool:$6, L2
$7 = u32:size + u32:20
$0 = u32:$7
jump L1
label L2
$10 = u32:0 <= u32:size
branch.false bool:$10, L8
$11 = u32:size < u32:3
branch.false bool:$11, L8
label L9
$0 = u32:10
jump L1
label L8
label L13
$14 = u32:size == u32:10
branch.false bool:$14, L12
$15 = u32:size * u32:10
$0 = u32:$15
jump L1
label L12
$16 = u32:size + u32:100
$0 = u32:$16
label L1
return u32:$0
end
fn main
string.reset
$0 = string.start
string.append string:"1: "
$2 = call fn(u32)->u32:score, u32:1
string.append u32:$2
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$3 = string.start
string.append string:"2: "
$5 = call fn(u32)->u32:score, u32:2
string.append u32:$5
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$6 = string.start
string.append string:"10: "
$8 = call fn(u32)->u32:score, u32:10
string.append u32:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$9 = string.start
string.append string:"50: "
$11 = call fn(u32)->u32:score, u32:50
string.append u32:$11
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
return i32:0
end
¬
void init() {}
uint32_t $score(uint32_t $size) {
    uint32_t $0 = 0;
    bool $4 = 0 <= $size;
    if (!$4) goto L2;
    bool $5 = $size < 3;
    if (!$5) goto L2;
    L3: ;
    bool $6 = $size == 2;
    if (!$6) goto L2;
    uint32_t $7 = $size + 20;
    $0 = $7;
    goto L1;
    L2: ;
    bool $10 = 0 <= $size;
    if (!$10) goto L8;
    bool $11 = $size < 3;
    if (!$11) goto L8;
    L9: ;
    $0 = 10;
    goto L1;
    L8: ;
    L13: ;
    bool $14 = $size == 10;
    if (!$14) goto L12;
    uint32_t $15 = $size * 10;
    $0 = $15;
    goto L1;
    L12: ;
    uint32_t $16 = $size + 100;
    $0 = $16;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"1: ", .count = 3}));
    uint32_t $2 = $score(1);
    string_builder_append_string(to_string$u32($2));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"2: ", .count = 3}));
    uint32_t $5 = $score(2);
    string_builder_append_string(to_string$u32($5));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"10: ", .count = 4}));
    uint32_t $8 = $score(10);
    string_builder_append_string(to_string$u32($8));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $9 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"50: ", .count = 4}));
    uint32_t $11 = $score(50);
    string_builder_append_string(to_string$u32($11));
    string $10 = string_builder_finish($9);
    prn($10);
    string_builder_reset();
    return 0;
}
