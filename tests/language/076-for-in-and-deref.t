use std.io

main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    for _item in values[..] {
    }

    total := 0
    for item in values[..] {
        total += item^
    }

    text := "ab"
    count := 0
    sum := 0
    for ch in text {
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
global input_buf
global input_len
fn pr
param string:text
$0 = string:text.data
$1 = string:text.count
$2 = call fn(i32,^u8,usize)->isize:write, i32:1, ^u8:$0, usize:$1
return void:
end
fn epr
param string:text
$0 = string:text.data
$1 = string:text.count
$2 = call fn(i32,^u8,usize)->isize:write, i32:2, ^u8:$0, usize:$1
return void:
end
fn prn
param string:text
call fn(string)->void:pr, string:text
$0 = call fn(i32,^u8,usize)->isize:write, i32:1, ^u8:"\n", usize:1
return void:
end
fn eprn
param string:text
call fn(string)->void:epr, string:text
$0 = call fn(i32,^u8,usize)->isize:write, i32:2, ^u8:"\n", usize:1
return void:
end
fn input
param string:prompt
call fn(string)->void:pr, string:prompt
$0 = [256]u8:input_buf[..]
$1 = []u8:$0.data
$2 = call fn(^u8,...)->i32:scanf, ^u8:"%255[^\n]", ^u8:$1
local read_count = i32:$2
$3 = call fn()->i32:getchar
local ch = i32:$3
label L4
local $6 = bool:no
$9 = i32:ch != i32:10
branch.false bool:$9, L7
$10 = i32:ch != i32:-1
branch.false bool:$10, L7
$6 = bool:yes
jump L8
label L7
$6 = bool:no
label L8
branch.false bool:$6, L5
block
$11 = call fn()->i32:getchar
ch = i32:$11
end
jump L4
label L5
$12 = i32:read_count == i32:1
local $13 = usize:0
branch.false bool:$12, L15
$16 = [256]u8:input_buf[..]
$17 = []u8:$16.data
$18 = call fn(^u8)->usize:strlen, ^u8:$17
$13 = usize:$18
jump L14
label L15
$13 = usize:0
label L14
local input_len = usize:$13
$19 = [256]u8:input_buf[i32:0..usize:input_len]
$20 = cast []u8:$19
return string:$20
end
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
$9 = ^[]i32:$2[$3]
local main$_item$for28 = ^i32:$9
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
$19 = ^[]i32:$12[$13]
local main$item$for41 = ^i32:$19
block
$20 = ^i32:main$item$for41[0]
$21 = i32:total + i32:$20
total = i32:$21
end
label L15
$22 = $13 + 1
$13 = $22
jump L14
label L16
local text = string:"ab"
local count = i32:0
local sum = i32:0
local $23 = string:0
$23 = string:text
local $24 = 0
$24 = 0
label L25
$28 = string:$23.count
$29 = $24 < $28
branch.false bool:$29, L27
$30 = ^string:$23[$24]
local main$ch$for66 = ^u8:$30
block
$31 = i32:count + i32:1
count = i32:$31
$32 = ^u8:main$ch$for66[0]
$33 = cast u8:$32
$34 = i32:sum + i32:$33
sum = i32:$34
end
label L26
$35 = $24 + 1
$24 = $35
jump L25
label L27
$37 = ^[3]i32:values
$36 = ^^[3]i32:$37^[i32:1]
local ptr = ^i32:$36
$38 = string.start
$40 = ^i32:ptr[0]
string.append i32:$40
string.append string:" "
string.append i32:total
string.append string:" "
string.append i32:count
string.append string:" "
string.append i32:sum
$39 = string.finish $38
call fn(string)->void:prn, string:$39
string.reset
$41 = ^i32:ptr[0]
$42 = i32:$41 + i32:total
$43 = i32:$42 + i32:count
return i32:$43
end
init
input_buf = [256]u8:0
input_len = usize:0
end
¬
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef uint8_t u8;

#define DEF_SLICE(type)                                                        \
    typedef struct {                                                           \
        type*  data;                                                           \
        size_t count;                                                          \
    }

DEF_SLICE(u8) string;

static void epr(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void eprn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    epr("\n");
}

void nerd_assert(bool condition,
                 const char* source_path,
                 unsigned    line,
                 string      message)
{
    if (condition) {
        return;
    }
    epr("assertion failed at %s:%u: ", source_path, line);
    fwrite(message.data, 1, message.count, stderr);
    epr("\n");
    exit(127);
}

bool string_eq(string lhs, string rhs)
{
    if (lhs.count != rhs.count) {
        return false;
    }
    if (lhs.count == 0) {
        return true;
    }
    return memcmp(lhs.data, rhs.data, lhs.count) == 0;
}

static bool string_is_utf8_boundary(string value, size_t index)
{
    if (index > value.count) {
        return false;
    }
    return index == value.count || (value.data[index] & 0xC0) != 0x80;
}

static string string_slice(string value, size_t start, size_t end)
{
    if (start > end || end > value.count ||
        !string_is_utf8_boundary(value, start) ||
        !string_is_utf8_boundary(value, end)) {
        eprn("fatal: string slice out of bounds");
        abort();
    }

    return (string){.data = value.data + start, .count = end - start};
}

#define NERD_STRING_ARENA_CAPACITY (64 * 1024)

static _Thread_local u8     g_string_arena[NERD_STRING_ARENA_CAPACITY];
static _Thread_local size_t g_string_arena_cursor = 0;

void string_builder_reset(void) { g_string_arena_cursor = 0; }

size_t string_builder_mark(void) { return g_string_arena_cursor; }

void string_builder_append_string(string str)
{
    if (g_string_arena_cursor + str.count > NERD_STRING_ARENA_CAPACITY) {
        eprn("fatal: string arena overflow");
        abort();
    }

    if (str.count > 0) {
        memcpy(g_string_arena + g_string_arena_cursor, str.data, str.count);
        g_string_arena_cursor += str.count;
    }
}

void string_builder_append_byte(u8 byte)
{
    if (g_string_arena_cursor + 1 > NERD_STRING_ARENA_CAPACITY) {
        eprn("fatal: string arena overflow");
        abort();
    }

    g_string_arena[g_string_arena_cursor++] = byte;
}

string string_builder_finish(size_t start)
{
    return (string){.data = g_string_arena + start,
                    .count = g_string_arena_cursor - start};
}

string to_string$string(string value) { return value; }

string to_string$bool(bool value)
{
    return value ? (string){.data = (u8*)"yes", .count = 3}
                 : (string){.data = (u8*)"no", .count = 2};
}

#define DEF_TO_STRING_INT(name, type, format, cast_type)                       \
    string to_string$##name(type value)                                        \
    {                                                                          \
        static u8 buffer[64];                                                  \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), format, (cast_type)value); \
        if (count < 0) {                                                       \
            return (string){0};                                                \
        }                                                                      \
        return (string){.data = buffer, .count = (size_t)count};               \
    }

#define DEF_TO_STRING_FLOAT(name, type)                                        \
    string to_string$##name(type value)                                        \
    {                                                                          \
        static u8 buffer[64];                                                  \
        int count =                                                            \
            snprintf((char*)buffer, sizeof(buffer), "%g", (double)value);      \
        if (count < 0) {                                                       \
            return (string){0};                                                \
        }                                                                      \
        return (string){.data = buffer, .count = (size_t)count};               \
    }

DEF_TO_STRING_INT(i8, int8_t, "%d", int)
DEF_TO_STRING_INT(i16, int16_t, "%d", int)
DEF_TO_STRING_INT(i32, int, "%d", int)
DEF_TO_STRING_INT(i64, int64_t, "%lld", long long)
DEF_TO_STRING_INT(u8, uint8_t, "%u", unsigned int)
DEF_TO_STRING_INT(u16, uint16_t, "%u", unsigned int)
DEF_TO_STRING_INT(u32, uint32_t, "%u", unsigned int)
DEF_TO_STRING_INT(u64, uint64_t, "%llu", unsigned long long)
DEF_TO_STRING_INT(isize, intptr_t, "%td", ptrdiff_t)
DEF_TO_STRING_INT(usize, uintptr_t, "%zu", size_t)
DEF_TO_STRING_FLOAT(f32, float)
DEF_TO_STRING_FLOAT(f64, double)

#ifndef NERD_TYPE_array0c21e4e1
#define NERD_TYPE_array0c21e4e1
typedef struct array0c21e4e1 {
    uint8_t items[256];
} array0c21e4e1;
#endif
#ifndef NERD_TYPE_slice6d447031
#define NERD_TYPE_slice6d447031
typedef struct slice6d447031 {
    uint8_t* data;
    uintptr_t count;
} slice6d447031;
#endif
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
int printf(const char*, ...);
int scanf(const char*, ...);
uintptr_t strlen(const char*);
int getchar(void);
intptr_t write(int, const char*, uintptr_t);

array0c21e4e1 $input_buf;
uintptr_t $input_len;
void $pr(string $text) {
    uint8_t* $0 = $text.data;
    uintptr_t $1 = $text.count;
    intptr_t $2 = write(1, (const char*)$0, $1);
    return;
}
void $epr(string $text) {
    uint8_t* $0 = $text.data;
    uintptr_t $1 = $text.count;
    intptr_t $2 = write(2, (const char*)$0, $1);
    return;
}
void $prn(string $text) {
    $pr($text);
    intptr_t $0 = write(1, (const char*)(u8*)"\n", 1);
    return;
}
void $eprn(string $text) {
    $epr($text);
    intptr_t $0 = write(2, (const char*)(u8*)"\n", 1);
    return;
}
string $input(string $prompt) {
    $pr($prompt);
    slice6d447031 $0 = (slice6d447031){.data = $input_buf.items, .count = 256};
    uint8_t* $1 = $0.data;
    int $2 = scanf((const char*)(u8*)"%255[^\n]", $1);
    int $read_count = $2;
    int $3 = getchar();
    int $ch = $3;
    L4: ;
    bool $6 = false;
    bool $9 = $ch != 10;
    if (!$9) goto L7;
    bool $10 = $ch != -1;
    if (!$10) goto L7;
    $6 = true;
    goto L8;
    L7: ;
    $6 = false;
    L8: ;
    if (!$6) goto L5;
    {
        int $11 = getchar();
        $ch = $11;
    }
    goto L4;
    L5: ;
    bool $12 = $read_count == 1;
    uintptr_t $13 = 0;
    if (!$12) goto L15;
    slice6d447031 $16 = (slice6d447031){.data = $input_buf.items, .count = 256};
    uint8_t* $17 = $16.data;
    uintptr_t $18 = strlen((const char*)$17);
    $13 = $18;
    goto L14;
    L15: ;
    $13 = 0;
    L14: ;
    uintptr_t $input_len = $13;
    slice6d447031 $19 = (slice6d447031){.data = $input_buf.items + 0, .count = ($input_len) - (0)};
    string $20 = (string){.data = $19.data, .count = $19.count};
    return $20;
}
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
    int* $9 = &$2.data[$3];
    int* $main$_item$for28 = $9;
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
    int* $19 = &$12.data[$13];
    int* $main$item$for41 = $19;
    {
        int $20 = $main$item$for41[0];
        int $21 = $total + $20;
        $total = $21;
    }
    L15: ;
    int $22 = $13 + 1;
    $13 = $22;
    goto L14;
    L16: ;
    string $text = (string){.data = (u8*)"ab", .count = 2};
    int $count = 0;
    int $sum = 0;
    string $23 = (string){0};
    $23 = $text;
    int $24 = 0;
    $24 = 0;
    L25: ;
    int $28 = $23.count;
    bool $29 = $24 < $28;
    if (!$29) goto L27;
    uint8_t* $30 = &$23.data[$24];
    uint8_t* $main$ch$for66 = $30;
    {
        int $31 = $count + 1;
        $count = $31;
        uint8_t $32 = $main$ch$for66[0];
        int $33 = (int)$32;
        int $34 = $sum + $33;
        $sum = $34;
    }
    L26: ;
    int $35 = $24 + 1;
    $24 = $35;
    goto L25;
    L27: ;
    struct arrayc02222e1* $37 = &$values;
    int* $36 = &$37->items[1];
    int* $ptr = $36;
    size_t $38 = string_builder_mark();
    int $40 = $ptr[0];
    string_builder_append_string(to_string$i32($40));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($total));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($count));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($sum));
    string $39 = string_builder_finish($38);
    $prn($39);
    string_builder_reset();
    int $41 = $ptr[0];
    int $42 = $41 + $total;
    int $43 = $42 + $count;
    return $43;
}
void init() {
    $input_buf = (array0c21e4e1){0};
    $input_len = 0;
}

int main() {
    init();
    return $main();
}
