adder := fn (a: i32, b: i32) => a + b
main :: fn () => adder(20, 22)
¬
42
¬

¬
global adder
fn adder$anon8
param i32:a
param i32:b
$0 = i32:a + i32:b
return i32:$0
end
fn main
$0 = call fn(i32,i32)->i32:adder, i32:20, i32:22
return i32:$0
end
init
adder = fn(i32,i32)->i32:adder$anon8
end
¬
int (*$adder)(int, int);
int $adder$anon8(int $a, int $b) {
    int $0 = $a + $b;
    return $0;
}
int $main() {
    int $0 = $adder(20, 22);
    return $0;
}
void init() {
    $adder = $adder$anon8;
}
