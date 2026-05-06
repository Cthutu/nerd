main :: fn () -> i32 {
    null_ptr: ^u8 = 0
    display_addr :: 0x1000
    display: ^void = display_addr
    other := 0x2000.as(^u8)

    on null_ptr != nil => return 1
    on display == nil => return 2
    on other == nil => return 3
    return 0
}
¬
0
¬
¬
fn main
local null_ptr = ^u8:0
local display = ^void:4096
$0 = cast i32:8192
local other = ^u8:$0
$1 = ^u8:null_ptr != ^u8:0
branch.false bool:$1, L2
return i32:1
label L2
$3 = ^void:display == ^void:0
branch.false bool:$3, L4
return i32:2
label L4
$5 = ^u8:other == ^u8:0
branch.false bool:$5, L6
return i32:3
label L6
return i32:0
end
¬
