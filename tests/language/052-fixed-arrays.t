main :: fn () -> i32 {
    xs :: [1, 2, 3]
    ys: [2]i32 = [4, 5]

    prn($"xs = {xs[0]}, {xs[2]}")
    prn($"xs array = {xs}")
    prn($"ys = {ys[1]}")
    prn($"ys array = {ys}")

    return xs[1] + ys[0] + ys[1]
}

¬
11
¬
xs = 1, 3
xs array = [1, 2, 3]
ys = 5
ys array = [4, 5]

¬
fn main
string.reset
$0 = array[i32:4, i32:5]
local ys = [2]i32:$0
$1 = string.start
string.append string:"xs = "
$3 = array[i32:1, i32:2, i32:3]
$4 = [3]i32:$3[i32:0]
string.append i32:$4
string.append string:", "
$5 = [3]i32:$3[i32:2]
string.append i32:$5
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
$6 = string.start
string.append string:"xs array = "
string.append [3]i32:$3
$7 = string.finish $6
call fn(string)->void:prn, string:$7
string.reset
$8 = string.start
string.append string:"ys = "
$10 = [2]i32:ys[i32:1]
string.append i32:$10
$9 = string.finish $8
call fn(string)->void:prn, string:$9
string.reset
$11 = string.start
string.append string:"ys array = "
string.append [2]i32:ys
$12 = string.finish $11
call fn(string)->void:prn, string:$12
string.reset
$13 = [3]i32:$3[i32:1]
$14 = [2]i32:ys[i32:0]
$15 = i32:$13 + i32:$14
$16 = [2]i32:ys[i32:1]
$17 = i32:$15 + i32:$16
return i32:$17
end
¬
void init() {}
typedef struct array8 {
    int items[2];
} array8;
typedef struct array9 {
    int items[3];
} array9;
int $main() {
    string_builder_reset();
    array8 $0 = (array8){.items = {4, 5}};
    array8 $ys = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"xs = ", .count = 5}));
    array9 $3 = (array9){.items = {1, 2, 3}};
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 3) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $4 = $3.items[0];
    string_builder_append_string(to_string$i32($4));
    string_builder_append_string(to_string$string((string){.data = (u8*)", ", .count = 2}));
    #ifndef NDEBUG
    if ((long long)2 < 0 || (size_t)2 >= 3) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $5 = $3.items[2];
    string_builder_append_string(to_string$i32($5));
    string $2 = string_builder_finish($1);
    prn($2);
    string_builder_reset();
    size_t $6 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"xs array = ", .count = 11}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    string_builder_append_string(to_string$i32($3.items[0]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($3.items[1]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($3.items[2]));
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $7 = string_builder_finish($6);
    prn($7);
    string_builder_reset();
    size_t $8 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"ys = ", .count = 5}));
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $10 = $ys.items[1];
    string_builder_append_string(to_string$i32($10));
    string $9 = string_builder_finish($8);
    prn($9);
    string_builder_reset();
    size_t $11 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"ys array = ", .count = 11}));
    string_builder_append_string((string){.data = (u8*)"[", .count = 1});
    string_builder_append_string(to_string$i32($ys.items[0]));
    string_builder_append_string((string){.data = (u8*)", ", .count = 2});
    string_builder_append_string(to_string$i32($ys.items[1]));
    string_builder_append_string((string){.data = (u8*)"]", .count = 1});
    string $12 = string_builder_finish($11);
    prn($12);
    string_builder_reset();
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 3) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $13 = $3.items[1];
    #ifndef NDEBUG
    if ((long long)0 < 0 || (size_t)0 >= 2) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $14 = $ys.items[0];
    int $15 = $13 + $14;
    #ifndef NDEBUG
    if ((long long)1 < 0 || (size_t)1 >= 2) { fprintf(stderr, "fatal: array index out of bounds\n"); abort(); }
    #endif
    int $16 = $ys.items[1];
    int $17 = $15 + $16;
    return $17;
}
