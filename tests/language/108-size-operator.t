use std.io

helper :: fn () {}

main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    i32_size := i32.size
    literal_size := 128.size
    array_size := nums.size
    slice_size := view.size
    string_size := text.size
    ptr_size := ptr.size
    nil_size := nil.size
    fn_size := helper.size
    void_size := void.size
    prn($"i32={i32_size} literal={literal_size} array={array_size} slice={slice_size} string={string_size} ptr={ptr_size} nil={nil_size} fn={fn_size} void={void_size}")
    total := i32_size + literal_size + array_size + slice_size + string_size +
        ptr_size + nil_size + fn_size + void_size
    return total.as(i32)
}
¬
68
¬
i32=4 literal=4 array=12 slice=16 string=16 ptr=8 nil=0 fn=8 void=0

¬
fn helper
return i32:0
end
fn main
string.reset
$0 = array[i32:1, i32:2, i32:3]
local nums = [3]i32:$0
$1 = [3]i32:nums[..]
local view = []i32:$1
local ptr = ^i32:0
$2 = size i32
local i32_size = usize:$2
$3 = size i32
local literal_size = usize:$3
$4 = size [3]i32
local array_size = usize:$4
$5 = size []i32
local slice_size = usize:$5
$6 = size string
local string_size = usize:$6
$7 = size ^i32
local ptr_size = usize:$7
$8 = size nil
local nil_size = usize:$8
$9 = size fn()->i32
local fn_size = usize:$9
$10 = size void
local void_size = usize:$10
$11 = string.start
string.append string:"i32="
string.append usize:i32_size
string.append string:" literal="
string.append usize:literal_size
string.append string:" array="
string.append usize:array_size
string.append string:" slice="
string.append usize:slice_size
string.append string:" string="
string.append usize:string_size
string.append string:" ptr="
string.append usize:ptr_size
string.append string:" nil="
string.append usize:nil_size
string.append string:" fn="
string.append usize:fn_size
string.append string:" void="
string.append usize:void_size
$12 = string.finish $11
call fn(string)->void:prn, string:$12
string.reset
$13 = usize:i32_size + usize:literal_size
$14 = usize:$13 + usize:array_size
$15 = usize:$14 + usize:slice_size
$16 = usize:$15 + usize:string_size
$17 = usize:$16 + usize:ptr_size
$18 = usize:$17 + usize:nil_size
$19 = usize:$18 + usize:fn_size
$20 = usize:$19 + usize:void_size
local total = usize:$20
$21 = cast usize:total
return i32:$21
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
int $helper() {
    return 0;
}
int $main() {
    string_builder_reset();
    arrayc02222e1 $0 = (arrayc02222e1){.items = {1, 2, 3}};
    arrayc02222e1 $nums = $0;
    slice3087b0d5 $1 = (slice3087b0d5){.data = $nums.items, .count = 3};
    slice3087b0d5 $view = $1;
    int* $ptr = NULL;
    uintptr_t $2 = sizeof(int);
    uintptr_t $i32_size = $2;
    uintptr_t $3 = sizeof(int);
    uintptr_t $literal_size = $3;
    uintptr_t $4 = sizeof(arrayc02222e1);
    uintptr_t $array_size = $4;
    uintptr_t $5 = sizeof(slice3087b0d5);
    uintptr_t $slice_size = $5;
    uintptr_t $6 = sizeof(string);
    uintptr_t $string_size = $6;
    uintptr_t $7 = sizeof(int*);
    uintptr_t $ptr_size = $7;
    uintptr_t $8 = 0;
    uintptr_t $nil_size = $8;
    uintptr_t $9 = sizeof(void*);
    uintptr_t $fn_size = $9;
    uintptr_t $10 = 0;
    uintptr_t $void_size = $10;
    size_t $11 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"i32=", .count = 4}));
    string_builder_append_string(to_string$usize($i32_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" literal=", .count = 9}));
    string_builder_append_string(to_string$usize($literal_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" array=", .count = 7}));
    string_builder_append_string(to_string$usize($array_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" slice=", .count = 7}));
    string_builder_append_string(to_string$usize($slice_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" string=", .count = 8}));
    string_builder_append_string(to_string$usize($string_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ptr=", .count = 5}));
    string_builder_append_string(to_string$usize($ptr_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" nil=", .count = 5}));
    string_builder_append_string(to_string$usize($nil_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" fn=", .count = 4}));
    string_builder_append_string(to_string$usize($fn_size));
    string_builder_append_string(to_string$string((string){.data = (u8*)" void=", .count = 6}));
    string_builder_append_string(to_string$usize($void_size));
    string $12 = string_builder_finish($11);
    prn($12);
    string_builder_reset();
    uintptr_t $13 = $i32_size + $literal_size;
    uintptr_t $14 = $13 + $array_size;
    uintptr_t $15 = $14 + $slice_size;
    uintptr_t $16 = $15 + $string_size;
    uintptr_t $17 = $16 + $ptr_size;
    uintptr_t $18 = $17 + $nil_size;
    uintptr_t $19 = $18 + $fn_size;
    uintptr_t $20 = $19 + $void_size;
    uintptr_t $total = $20;
    int $21 = (int)$total;
    return $21;
}
