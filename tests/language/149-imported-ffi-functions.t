x :: use test.ffi_import

main :: fn () -> i32 {
    return x.absolute(-9)
}
¬
9
¬
¬
fn main
$0 = call fn(i32)->i32:abs, i32:-9
return i32:$0
end
¬
void init() {}
int abs(int);

int $main() {
    int $0 = abs(-9);
    return $0;
}
