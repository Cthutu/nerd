use mod std.print

-- Matches exclusive and inclusive integer ranges in block-form `on` branches.
test_branch :: fn (size: u32) -> i32 {
    return on size {
        0..2 => 10
        2..=4 => 20
        else => 30
    }
}

main :: fn () {
    prn($"0: {test_branch(0)}")
    prn($"1: {test_branch(1)}")
    prn($"2: {test_branch(2)}")
    prn($"4: {test_branch(4)}")
    prn($"5: {test_branch(5)}")

    return test_branch(5)
}
¬
30
¬
0: 10
1: 10
2: 20
4: 20
5: 30

¬
fn test_branch
param u32:size
local $0 = i32:0
$4 = u32:0 <= u32:size
branch.false bool:$4, L2
$5 = u32:size < u32:2
branch.false bool:$5, L2
label L3
$0 = i32:10
jump L1
label L2
$8 = u32:2 <= u32:size
branch.false bool:$8, L6
$9 = u32:size <= u32:4
branch.false bool:$9, L6
label L7
$0 = i32:20
jump L1
label L6
$0 = i32:30
label L1
return i32:$0
end
fn main
string.reset
$0 = string.start
string.append string:"0: "
$2 = call fn(u32)->i32:test_branch, u32:0
string.append i32:$2
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$3 = string.start
string.append string:"1: "
$5 = call fn(u32)->i32:test_branch, u32:1
string.append i32:$5
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$6 = string.start
string.append string:"2: "
$8 = call fn(u32)->i32:test_branch, u32:2
string.append i32:$8
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$9 = string.start
string.append string:"4: "
$11 = call fn(u32)->i32:test_branch, u32:4
string.append i32:$11
$10 = string.finish $9
call fn(string)->void:prn, string:$10
string.reset
$12 = string.start
string.append string:"5: "
$14 = call fn(u32)->i32:test_branch, u32:5
string.append i32:$14
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$15 = call fn(u32)->i32:test_branch, u32:5
return i32:$15
end
¬
void init() {}
int $test_branch(uint32_t $size) {
    int $0 = 0;
    bool $4 = 0 <= $size;
    if (!$4) goto L2;
    bool $5 = $size < 2;
    if (!$5) goto L2;
    L3: ;
    $0 = 10;
    goto L1;
    L2: ;
    bool $8 = 2 <= $size;
    if (!$8) goto L6;
    bool $9 = $size <= 4;
    if (!$9) goto L6;
    L7: ;
    $0 = 20;
    goto L1;
    L6: ;
    $0 = 30;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"0: ", .count = 3}));
    int $2 = $test_branch(0);
    string_builder_append_string(to_string$i32($2));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"1: ", .count = 3}));
    int $5 = $test_branch(1);
    string_builder_append_string(to_string$i32($5));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"2: ", .count = 3}));
    int $8 = $test_branch(2);
    string_builder_append_string(to_string$i32($8));
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $9 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"4: ", .count = 3}));
    int $11 = $test_branch(4);
    string_builder_append_string(to_string$i32($11));
    string $10 = string_builder_finish($9);
    prn($10);
    string_builder_reset();
    size_t $12 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"5: ", .count = 3}));
    int $14 = $test_branch(5);
    string_builder_append_string(to_string$i32($14));
    string $13 = string_builder_finish($12);
    prn($13);
    string_builder_reset();
    int $15 = $test_branch(5);
    return $15;
}
