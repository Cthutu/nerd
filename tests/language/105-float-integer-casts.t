-- Casts between i32 and f32, including float-to-integer truncation toward zero.
main :: fn () -> i32 {
    up: f32 = 42.as(f32)
    down: f32 = 3.9
    below: f32 = -3.9

    whole := down.as(i32)
    negative := below.as(i32)
    return up.as(i32) + whole + negative
}
¬
42
¬
¬
fn main
$0 = cast i32:42
local up = f32:$0
local down = f32:3.8999999999999999
$1 = -f32:3.8999999999999999
local below = f32:$1
$2 = cast f32:down
local whole = i32:$2
$3 = cast f32:below
local negative = i32:$3
$4 = cast f32:up
$5 = i32:$4 + i32:whole
$6 = i32:$5 + i32:negative
return i32:$6
end
¬
void init() {}
int $main() {
    float $0 = (float)42;
    float $up = $0;
    float $down = 3.8999999999999999f;
    float $1 = -3.8999999999999999f;
    float $below = $1;
    int $2 = (int)$down;
    int $whole = $2;
    int $3 = (int)$below;
    int $negative = $3;
    int $4 = (int)$up;
    int $5 = $4 + $whole;
    int $6 = $5 + $negative;
    return $6;
}
