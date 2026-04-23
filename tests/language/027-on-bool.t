-- Chooses between two values with a short-form boolean `on` expression.
enabled: bool = true

main :: fn () => on enabled => 42 else 7
¬
42
¬

¬
global enabled
fn main
local $0 = i32:0
branch.false bool:enabled, L1
$0 = i32:42
jump L2
label L1
$0 = i32:7
label L2
return i32:$0
end
init
enabled = bool:true
end
¬
bool $enabled;
int $main() {
    int $0 = 0;
    if (!$enabled) goto L1;
    $0 = 42;
    goto L2;
    L1: ;
    $0 = 7;
    L2: ;
    return $0;
}
void init() {
    $enabled = true;
}
