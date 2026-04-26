use std.print

main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 30]
    array_ptr: ^[3]i32 = ^values
    elem_ptr: ^i32 = ^values[1]
    literal_ptr: ^[3]i32 = ^[1, 2, 3]

    prn($"array pointer = {array_ptr[0]}")
    prn($"elem pointer = {elem_ptr[0]}")
    prn($"literal pointer = {literal_ptr[0]}")

    return array_ptr[0][0] + elem_ptr[0] + literal_ptr[0][2]
}
¬
33
¬
array pointer = [10, 20, 30]
elem pointer = 20
literal pointer = [1, 2, 3]

¬
fn main
string.reset
$0 = array[i32:10, i32:20, i32:30]
local values = [3]i32:$0
$1 = ^[3]i32:values
local array_ptr = ^[3]i32:$1
$2 = ^[3]i32:values[i32:1]
local elem_ptr = ^i32:$2
$4 = array[i32:1, i32:2, i32:3]
$3 = ^[3]i32:$4
local literal_ptr = ^[3]i32:$3
$5 = string.start
string.append string:"array pointer = "
$7 = ^[3]i32:array_ptr[i32:0]
string.append [3]i32:$7
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$8 = string.start
string.append string:"elem pointer = "
$10 = ^i32:elem_ptr[i32:0]
string.append i32:$10
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$11 = string.start
string.append string:"literal pointer = "
$13 = ^[3]i32:literal_ptr[i32:0]
string.append [3]i32:$13
$12 = string.finish $11
call fn(string)->void:prn, string:$12
string.reset
$14 = ^[3]i32:array_ptr[i32:0]
$15 = [3]i32:$14[i32:0]
$16 = ^i32:elem_ptr[i32:0]
$17 = i32:$15 + i32:$16
$18 = ^[3]i32:literal_ptr[i32:0]
$19 = [3]i32:$18[i32:2]
$20 = i32:$17 + i32:$19
return i32:$20
end
¬
void init() {}
typedef struct array10 {
    int items[3];
} array10;
int $main() {
    string_builder_reset();
    array10 $0 = (array10){.items = {10, 20, 30}};
    array10 $values = $0;
    array10* $1 = &$values;
    array10* $array_ptr = $1;
    int* $2 = &$values.items[1];
    int* $elem_ptr = $2;
    array10 $4 = (array10){.items = {1, 2, 3}};
    array10* $3 = &$4;
    array10* $literal_ptr = $3;
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"array pointer = ", .count = 16}));
    array10 $7 = $array_ptr[0];
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    string_builder_append_string(to_string$i32($7.items[0]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($7.items[1]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($7.items[2]));
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $6 = string_builder_finish($5);
    prn($6);
    string_builder_reset();
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"elem pointer = ", .count = 15}));
    int $10 = $elem_ptr[0];
    string_builder_append_string(to_string$i32($10));
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    size_t $11 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"literal pointer = ", .count = 18}));
    array10 $13 = $literal_ptr[0];
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    string_builder_append_string(to_string$i32($13.items[0]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($13.items[1]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($13.items[2]));
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $12 = string_builder_finish($11);
    prn($12);
    string_builder_reset();
    array10 $14 = $array_ptr[0];
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 3) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $15 = $14.items[0];
    int $16 = $elem_ptr[0];
    int $17 = $15 + $16;
    array10 $18 = $literal_ptr[0];
    #ifndef NDEBUG
    if ((long long)2 < 0 || (size_t)2 >= 3) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $19 = $18.items[2];
    int $20 = $17 + $19;
    return $20;
}
