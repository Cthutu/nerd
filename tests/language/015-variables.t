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
init
base = 40
end
fn main
$0 = base + 1
base = $0
local extra = 0
extra = 1
$1 = base + extra
return $1
end
¬
int $base;
void init() {
    $base = 40;
}
int $main() {
    int $0 = $base + 1;
    $base = $0;
    int $extra = 0;
    $extra = 1;
    int $1 = $base + $extra;
    return $1;
}
