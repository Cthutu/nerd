on linux {
    use test.platform_ffi
}

main :: fn () -> i32 {
    return abs(-7) + PLATFORM_VALUE
}
¬
19
¬
¬
fn main
$0 = call fn(i32)->i32:abs, i32:-7
$1 = i32:$0 + i32:12
return i32:$1
end
¬
void init() {}
int abs(int);
int abs(int);

int $main() {
    int $0 = abs(-7);
    int $1 = $0 + 12;
    return $1;
}
