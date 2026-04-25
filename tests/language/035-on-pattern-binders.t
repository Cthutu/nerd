use std.print

-- Binds the matched value inside each block-form `on` branch.
size :: 2

test_branch :: fn (size: u32) -> u32 {
    return on size {
        0, 1 as matched => matched + 10
        2..=4 as ranged => ranged + 20
        else as other => other + 30
    }
}

main :: fn () {
    prn($"0: {test_branch(0)}")
    prn($"2: {test_branch(2)}")
    prn($"5: {test_branch(5)}")

    return test_branch(size).as(i32)
}
¬
22
¬
0: 10
2: 22
5: 35

¬
fn test_branch
param u32:size
local $0 = u32:0
$5 = u32:size == u32:0
branch.false bool:$5, L4
jump L3
label L4
$6 = u32:size == u32:1
branch.false bool:$6, L2
label L3
$7 = u32:size + u32:10
$0 = u32:$7
jump L1
label L2
$10 = u32:2 <= u32:size
branch.false bool:$10, L8
$11 = u32:size <= u32:4
branch.false bool:$11, L8
label L9
$12 = u32:size + u32:20
$0 = u32:$12
jump L1
label L8
$13 = u32:size + u32:30
$0 = u32:$13
label L1
return u32:$0
end
fn main
string.reset
$0 = string.start
string.append string:"0: "
$2 = call fn(u32)->u32:test_branch, u32:0
string.append u32:$2
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$3 = string.start
string.append string:"2: "
$5 = call fn(u32)->u32:test_branch, u32:2
string.append u32:$5
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$6 = string.start
string.append string:"5: "
$8 = call fn(u32)->u32:test_branch, u32:5
string.append u32:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$9 = call fn(u32)->u32:test_branch, u32:2
$10 = cast u32:$9
return i32:$10
end
¬
void init() {}
uint32_t $test_branch(uint32_t $size) {
    uint32_t $0 = 0;
    bool $5 = $size == 0;
    if (!$5) goto L4;
    goto L3;
    L4: ;
    bool $6 = $size == 1;
    if (!$6) goto L2;
    L3: ;
    uint32_t $7 = $size + 10;
    $0 = $7;
    goto L1;
    L2: ;
    bool $10 = 2 <= $size;
    if (!$10) goto L8;
    bool $11 = $size <= 4;
    if (!$11) goto L8;
    L9: ;
    uint32_t $12 = $size + 20;
    $0 = $12;
    goto L1;
    L8: ;
    uint32_t $13 = $size + 30;
    $0 = $13;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"0: ", .count = 3}));
    uint32_t $2 = $test_branch(0);
    string_builder_append_string(to_string$u32($2));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"2: ", .count = 3}));
    uint32_t $5 = $test_branch(2);
    string_builder_append_string(to_string$u32($5));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"5: ", .count = 3}));
    uint32_t $8 = $test_branch(5);
    string_builder_append_string(to_string$u32($8));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    uint32_t $9 = $test_branch(2);
    int $10 = (int)$9;
    return $10;
}
