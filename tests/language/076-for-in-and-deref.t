use std.io

main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    for _item in values[..] {
    }

    total := 0
    for item in values[..] {
        total += item
    }

    text := "ab"
    count := 0
    sum := 0
    for ^ch in text {
        count += 1
        sum += ch^.as(i32)
    }

    ptr: ^i32 = ^values[1]
    prn($"{ptr^} {total} {count} {sum}")
    return ptr^ + total + count
}
¬
10
¬
2 6 2 195

¬
fn main
string.reset
$0 = array[i32:1, i32:2, i32:3]
local values = [3]i32:$0
$1 = [3]i32:values[..]
local $2 = []i32:0
$2 = []i32:$1
local $3 = 0
$3 = 0
label L4
$7 = []i32:$2.count
$8 = $3 < $7
branch.false bool:$8, L6
$9 = []i32:$2[$3]
local main$_item$for28 = i32:$9
block
end
label L5
$10 = $3 + 1
$3 = $10
jump L4
label L6
local total = i32:0
$11 = [3]i32:values[..]
local $12 = []i32:0
$12 = []i32:$11
local $13 = 0
$13 = 0
label L14
$17 = []i32:$12.count
$18 = $13 < $17
branch.false bool:$18, L16
$19 = []i32:$12[$13]
local main$item$for41 = i32:$19
block
$20 = i32:total + i32:main$item$for41
total = i32:$20
end
label L15
$21 = $13 + 1
$13 = $21
jump L14
label L16
local text = string:"ab"
local count = i32:0
local sum = i32:0
local $22 = string:0
$22 = string:text
local $23 = 0
$23 = 0
label L24
$27 = string:$22.count
$28 = $23 < $27
branch.false bool:$28, L26
$29 = ^string:$22[$23]
local main$ch$for66 = ^u8:$29
block
$30 = i32:count + i32:1
count = i32:$30
$31 = ^u8:main$ch$for66[0]
$32 = cast u8:$31
$33 = i32:sum + i32:$32
sum = i32:$33
end
label L25
$34 = $23 + 1
$23 = $34
jump L24
label L26
$35 = ^[3]i32:values[i32:1]
local ptr = ^i32:$35
$36 = string.start
$38 = ^i32:ptr[0]
string.append i32:$38
string.append string:" "
string.append i32:total
string.append string:" "
string.append i32:count
string.append string:" "
string.append i32:sum
$37 = string.finish $36
call fn(string)->void:prn, string:$37
string.reset
$39 = ^i32:ptr[0]
$40 = i32:$39 + i32:total
$41 = i32:$40 + i32:count
return i32:$41
end
¬
void init() {}
#ifndef NERD_TYPE_arrayc02222e1
#define NERD_TYPE_arrayc02222e1
typedef struct arrayc02222e1 {
    int items[3];
} arrayc02222e1;
#endif
#ifndef NERD_TYPE_slice3087b0d5
#define NERD_TYPE_slice3087b0d5
typedef struct slice3087b0d5 {
    int* data;
    uintptr_t count;
} slice3087b0d5;
#endif
int $main() {
    string_builder_reset();
    arrayc02222e1 $0 = (arrayc02222e1){.items = {1, 2, 3}};
    arrayc02222e1 $values = $0;
    slice3087b0d5 $1 = (slice3087b0d5){.data = $values.items, .count = 3};
    slice3087b0d5 $2 = (slice3087b0d5){0};
    $2 = $1;
    int $3 = 0;
    $3 = 0;
    L4: ;
    int $7 = $2.count;
    bool $8 = $3 < $7;
    if (!$8) goto L6;
    #ifndef NDEBUG
    if ((long long)$3 < 0 || (size_t)$3 >= $2.count) { eprn("fatal: slice index out of bounds"); abort(); }
    #endif
    int $9 = $2.data[$3];
    int $main$_item$for28 = $9;
    {
    }
    L5: ;
    int $10 = $3 + 1;
    $3 = $10;
    goto L4;
    L6: ;
    int $total = 0;
    slice3087b0d5 $11 = (slice3087b0d5){.data = $values.items, .count = 3};
    slice3087b0d5 $12 = (slice3087b0d5){0};
    $12 = $11;
    int $13 = 0;
    $13 = 0;
    L14: ;
    int $17 = $12.count;
    bool $18 = $13 < $17;
    if (!$18) goto L16;
    #ifndef NDEBUG
    if ((long long)$13 < 0 || (size_t)$13 >= $12.count) { eprn("fatal: slice index out of bounds"); abort(); }
    #endif
    int $19 = $12.data[$13];
    int $main$item$for41 = $19;
    {
        int $20 = $total + $main$item$for41;
        $total = $20;
    }
    L15: ;
    int $21 = $13 + 1;
    $13 = $21;
    goto L14;
    L16: ;
    string $text = (string){.data = (u8*)"ab", .count = 2};
    int $count = 0;
    int $sum = 0;
    string $22 = (string){0};
    $22 = $text;
    int $23 = 0;
    $23 = 0;
    L24: ;
    int $27 = $22.count;
    bool $28 = $23 < $27;
    if (!$28) goto L26;
    uint8_t* $29 = &$22.data[$23];
    uint8_t* $main$ch$for66 = $29;
    {
        int $30 = $count + 1;
        $count = $30;
        uint8_t $31 = $main$ch$for66[0];
        int $32 = (int)$31;
        int $33 = $sum + $32;
        $sum = $33;
    }
    L25: ;
    int $34 = $23 + 1;
    $23 = $34;
    goto L24;
    L26: ;
    int* $35 = &$values.items[1];
    int* $ptr = $35;
    size_t $36 = string_builder_mark();
    int $38 = $ptr[0];
    string_builder_append_string(to_string$i32($38));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($total));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($count));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($sum));
    string $37 = string_builder_finish($36);
    prn($37);
    string_builder_reset();
    int $39 = $ptr[0];
    int $40 = $39 + $total;
    int $41 = $40 + $count;
    return $41;
}
