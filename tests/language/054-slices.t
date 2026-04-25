use mod std.print

main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    middle: []i32 = values[1..4]
    from_start: []i32 = values[..3]
    to_end: []i32 = values[2..]
    literal: []i32 = [1, 2, 3][..]
    reslice: []i32 = all[1..3]

    prn($"all = {all}")
    prn($"middle = {middle}")
    prn($"from_start = {from_start}")
    prn($"to_end = {to_end}")
    prn($"literal = {literal}")
    prn($"reslice count = {reslice.count}")
    prn($"data first = {middle.data[0]}")

    return middle[1] + from_start[2] + to_end[0] + literal[2]
}
¬
93
¬
all = [10, 20, 30, 40, 50]
middle = [20, 30, 40]
from_start = [10, 20, 30]
to_end = [30, 40, 50]
literal = [1, 2, 3]
reslice count = 2
data first = 20

¬
fn main
string.reset
$0 = array[i32:10, i32:20, i32:30, i32:40, i32:50]
local values = [5]i32:$0
$1 = [5]i32:values[..]
local all = []i32:$1
$2 = [5]i32:values[i32:1..i32:4]
local middle = []i32:$2
$3 = [5]i32:values[..i32:3]
local from_start = []i32:$3
$4 = [5]i32:values[i32:2..]
local to_end = []i32:$4
$5 = array[i32:1, i32:2, i32:3]
$6 = [3]i32:$5[..]
local literal = []i32:$6
$7 = []i32:all[i32:1..i32:3]
local reslice = []i32:$7
$8 = string.start
string.append string:"all = "
string.append []i32:all
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$10 = string.start
string.append string:"middle = "
string.append []i32:middle
$11 = string.finish $10
call fn(string)->void:prn, string:$11
string.reset
$12 = string.start
string.append string:"from_start = "
string.append []i32:from_start
$13 = string.finish $12
call fn(string)->void:prn, string:$13
string.reset
$14 = string.start
string.append string:"to_end = "
string.append []i32:to_end
$15 = string.finish $14
call fn(string)->void:prn, string:$15
string.reset
$16 = string.start
string.append string:"literal = "
string.append []i32:literal
$17 = string.finish $16
call fn(string)->void:prn, string:$17
string.reset
$18 = string.start
string.append string:"reslice count = "
$20 = []i32:reslice.count
string.append usize:$20
$19 = string.finish $18
call fn(string)->void:prn, string:$19
string.reset
$21 = string.start
string.append string:"data first = "
$23 = []i32:middle.data
$24 = ^i32:$23[i32:0]
string.append i32:$24
$22 = string.finish $21
call fn(string)->void:prn, string:$22
string.reset
$25 = []i32:middle[i32:1]
$26 = []i32:from_start[i32:2]
$27 = i32:$25 + i32:$26
$28 = []i32:to_end[i32:0]
$29 = i32:$27 + i32:$28
$30 = []i32:literal[i32:2]
$31 = i32:$29 + i32:$30
return i32:$31
end
¬
void init() {}
typedef struct array9 {
    int items[5];
} array9;
typedef struct slice10 {
    int* data;
    uintptr_t count;
} slice10;
typedef struct array11 {
    int items[3];
} array11;
int $main() {
    string_builder_reset();
    array9 $0 = (array9){.items = {10, 20, 30, 40, 50}};
    array9 $values = $0;
    slice10 $1 = (slice10){.data = $values.items, .count = 5};
    slice10 $all = $1;
    slice10 $2 = (slice10){.data = $values.items + 1, .count = (4) - (1)};
    slice10 $middle = $2;
    slice10 $3 = (slice10){.data = $values.items, .count = (3) - (0)};
    slice10 $from_start = $3;
    slice10 $4 = (slice10){.data = $values.items + 2, .count = 5 - 2};
    slice10 $to_end = $4;
    array11 $5 = (array11){.items = {1, 2, 3}};
    slice10 $6 = (slice10){.data = $5.items, .count = 3};
    slice10 $literal = $6;
    slice10 $7 = (slice10){.data = $all.data + 1, .count = (3) - (1)};
    slice10 $reslice = $7;
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"all = ", .count = 6}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    for (uintptr_t $slice_i0 = 0; $slice_i0 < $all.count; ++$slice_i0) {
        if ($slice_i0 > 0) string_builder_append_string((string){.data = (u8*)", ", .count = 2});
        string_builder_append_string(to_string$i32($all.data[$slice_i0]));
    }
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    size_t $10 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"middle = ", .count = 9}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    for (uintptr_t $slice_i1 = 0; $slice_i1 < $middle.count; ++$slice_i1) {
        if ($slice_i1 > 0) string_builder_append_string((string){.data = (u8*)", ", .count = 2});
        string_builder_append_string(to_string$i32($middle.data[$slice_i1]));
    }
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $11 = string_builder_finish($10);
    prn($11);
    string_builder_reset();
    size_t $12 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"from_start = ", .count = 13}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    for (uintptr_t $slice_i2 = 0; $slice_i2 < $from_start.count; ++$slice_i2) {
        if ($slice_i2 > 0) string_builder_append_string((string){.data = (u8*)", ", .count = 2});
        string_builder_append_string(to_string$i32($from_start.data[$slice_i2]));
    }
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $13 = string_builder_finish($12);
    prn($13);
    string_builder_reset();
    size_t $14 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"to_end = ", .count = 9}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    for (uintptr_t $slice_i3 = 0; $slice_i3 < $to_end.count; ++$slice_i3) {
        if ($slice_i3 > 0) string_builder_append_string((string){.data = (u8*)", ", .count = 2});
        string_builder_append_string(to_string$i32($to_end.data[$slice_i3]));
    }
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $15 = string_builder_finish($14);
    prn($15);
    string_builder_reset();
    size_t $16 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"literal = ", .count = 10}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    for (uintptr_t $slice_i4 = 0; $slice_i4 < $literal.count; ++$slice_i4) {
        if ($slice_i4 > 0) string_builder_append_string((string){.data = (u8*)", ", .count = 2});
        string_builder_append_string(to_string$i32($literal.data[$slice_i4]));
    }
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $17 = string_builder_finish($16);
    prn($17);
    string_builder_reset();
    size_t $18 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"reslice count = ", .count = 16}));
    uintptr_t $20 = $reslice.count;
    string_builder_append_string(to_string$usize($20));
    string $19 = string_builder_finish($18);
    prn($19);
    string_builder_reset();
    size_t $21 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"data first = ", .count = 13}));
    int* $23 = $middle.data;
    int $24 = $23[0];
    string_builder_append_string(to_string$i32($24));
    string $22 = string_builder_finish($21);
    prn($22);
    string_builder_reset();
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= $middle.count) { fprintf(stderr, "fatal: slice index out of bounds\n"); abort(); }
    #endif
    int $25 = $middle.data[1];
    #ifndef NDEBUG
    if ((long long)2 < 0 || (size_t)2 >= $from_start.count) { fprintf(stderr, "fatal: slice index out of bounds\n"); abort(); }
    #endif
    int $26 = $from_start.data[2];
    int $27 = $25 + $26;
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= $to_end.count) { fprintf(stderr, "fatal: slice index out of bounds\n"); abort(); }
    #endif
    int $28 = $to_end.data[0];
    int $29 = $27 + $28;
    #ifndef NDEBUG
    if ((long long)2 < 0 || (size_t)2 >= $literal.count) { fprintf(stderr, "fatal: slice index out of bounds\n"); abort(); }
    #endif
    int $30 = $literal.data[2];
    int $31 = $29 + $30;
    return $31;
}
