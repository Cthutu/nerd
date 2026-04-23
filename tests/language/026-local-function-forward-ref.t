-- Resolves a local function binding through a forward reference.
main :: fn () {
    result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b
    return result
}
¬
42
¬

¬
fn main$add
param i32:a
param i32:b
$0 = i32:a + i32:b
return i32:$0
end
fn main
$0 = call fn(i32,i32)->i32:main$add, i32:20, i32:22
local result = i32:$0
return i32:result
end
¬
void init() {}
int $main$add(int $a, int $b) {
    int $0 = $a + $b;
    return $0;
}
int $main() {
    int $0 = $main$add(20, 22);
    int $result = $0;
    return $result;
}
