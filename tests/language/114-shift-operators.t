-- Supports shift operators and compound shift assignment.
main :: fn () {
    x := 1
    y := 64
    x <<= 3
    y >>= 2
    return x + y + (2 << 4) + (128 >> 5)
}
¬
60
¬
¬
fn main
local x = i32:1
local y = i32:64
$0 = i32:x << i32:3
x = i32:$0
$1 = i32:y >> i32:2
y = i32:$1
$2 = i32:x + i32:y
$3 = i32:$2 + i32:32
$4 = i32:$3 + i32:4
return i32:$4
end
¬
void init() {}
int $main() {
    int $x = 1;
    int $y = 64;
    int $0 = $x << 3;
    $x = $0;
    int $1 = $y >> 2;
    $y = $1;
    int $2 = $x + $y;
    int $3 = $2 + 32;
    int $4 = $3 + 4;
    return $4;
}
