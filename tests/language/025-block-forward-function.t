add :: fn (a: i32, b: i32) -> i32 {
    return a + b
}

main :: fn () => add(20, 22)
¬
42
¬

¬
fn add
param i32:a
param i32:b
$0 = i32:a + i32:b
return i32:$0
end
fn main
$0 = call fn(i32,i32)->i32:add, i32:20, i32:22
return i32:$0
end
¬
void init() {}
int $add(int $a, int $b) {
    int $0 = $a + $b;
    return $0;
}
int $main() {
    int $0 = $add(20, 22);
    return $0;
}
