base := 40

main :: fn () {
    base = base + 1
    extra: i32
    extra = 1
    return base + extra
}
¬
42
¬

¬
global base
fn main
$0 = i32:base + i32:1
base = i32:$0
local extra = i32:0
extra = i32:1
$1 = i32:base + i32:extra
return i32:$1
end
init
base = i32:40
end
¬
int $base;
int $main() {
    int $0 = $base + 1;
    $base = $0;
    int $extra = 0;
    $extra = 1;
    int $1 = $base + $extra;
    return $1;
}
void init() {
    $base = 40;
}
