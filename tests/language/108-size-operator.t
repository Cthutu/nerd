helper :: fn () {}

main :: fn () -> i32 {
    nums: [3]i32 = [1, 2, 3]
    view: []i32 = nums[..]
    text :: "hi"
    ptr: ^i32 = nil
    total := i32.size + 128.size + nums.size + view.size + text.size +
        ptr.size + nil.size + helper.size + void.size
    return total.as(i32)
}
¬
68
¬

¬
fn helper
return i32:0
end
fn main
$0 = array[i32:1, i32:2, i32:3]
local nums = [3]i32:$0
$1 = [3]i32:nums[..]
local view = []i32:$1
local ptr = ^i32:0
$2 = size i32
$3 = size i32
$4 = usize:$2 + usize:$3
$5 = size [3]i32
$6 = usize:$4 + usize:$5
$7 = size []i32
$8 = usize:$6 + usize:$7
$9 = size string
$10 = usize:$8 + usize:$9
$11 = size ^i32
$12 = usize:$10 + usize:$11
$13 = size nil
$14 = usize:$12 + usize:$13
$15 = size fn()->i32
$16 = usize:$14 + usize:$15
$17 = size void
$18 = usize:$16 + usize:$17
local total = usize:$18
$19 = cast usize:total
return i32:$19
end
¬
void init() {}
typedef struct array9 {
    int items[3];
} array9;
typedef struct slice10 {
    int* data;
    uintptr_t count;
} slice10;
int $helper() {
    return 0;
}
int $main() {
    array9 $0 = (array9){.items = {1, 2, 3}};
    array9 $nums = $0;
    slice10 $1 = (slice10){.data = $nums.items, .count = 3};
    slice10 $view = $1;
    int* $ptr = NULL;
    uintptr_t $2 = sizeof(int);
    uintptr_t $3 = sizeof(int);
    uintptr_t $4 = $2 + $3;
    uintptr_t $5 = sizeof(array9);
    uintptr_t $6 = $4 + $5;
    uintptr_t $7 = sizeof(slice10);
    uintptr_t $8 = $6 + $7;
    uintptr_t $9 = sizeof(string);
    uintptr_t $10 = $8 + $9;
    uintptr_t $11 = sizeof(int*);
    uintptr_t $12 = $10 + $11;
    uintptr_t $13 = 0;
    uintptr_t $14 = $12 + $13;
    uintptr_t $15 = sizeof(void*);
    uintptr_t $16 = $14 + $15;
    uintptr_t $17 = 0;
    uintptr_t $18 = $16 + $17;
    uintptr_t $total = $18;
    int $19 = (int)$total;
    return $19;
}
