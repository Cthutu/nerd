-- Declares multiple foreign functions in one FFI block.
ffi "c" {
    abs (i32) -> i32
    strlen (^u8) -> usize
}

main :: fn () -> i32 {
    return abs(-7) + strlen(c"nerd").as(i32)
}
¬
11
¬
¬
fn main
$0 = call fn(i32)->i32:abs, i32:-7
$1 = call fn(^u8)->usize:strlen, ^u8:"nerd"
$2 = cast usize:$1
$3 = i32:$0 + i32:$2
return i32:$3
end
¬
void init() {}
int abs(int);
uintptr_t strlen(const char*);

int $main() {
    int $0 = abs(-7);
    uintptr_t $1 = strlen((const char*)(u8*)"nerd");
    int $2 = (int)$1;
    int $3 = $0 + $2;
    return $3;
}
