use mod std.print

-- Matches integer values with block-form `on` branches.
size :: 2

test_branch :: fn (size: u32) -> i32 {
    return on size {
        0 => 10
        1 => 20
        else => 30
    }
}

main :: fn () {
    prn($"0: {test_branch(0)}")
    prn($"1: {test_branch(1)}")
    prn($"2: {test_branch(2)}")

    return test_branch(size)
}
¬
30
¬
0: 10
1: 20
2: 30

¬
fn test_branch
param u32:size
local $0 = i32:0
$4 = u32:size == u32:0
branch.false bool:$4, L2
label L3
$0 = i32:10
jump L1
label L2
$7 = u32:size == u32:1
branch.false bool:$7, L5
label L6
$0 = i32:20
jump L1
label L5
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
$9 = call fn(u32)->i32:test_branch, u32:2
return i32:$9
end
¬
void init() {}
int $test_branch(uint32_t $size) {
    int $0 = 0;
    bool $4 = $size == 0;
    if (!$4) goto L2;
    L3: ;
    $0 = 10;
    goto L1;
    L2: ;
    bool $7 = $size == 1;
    if (!$7) goto L5;
    L6: ;
    $0 = 20;
    goto L1;
    L5: ;
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
    int $9 = $test_branch(2);
    return $9;
}
