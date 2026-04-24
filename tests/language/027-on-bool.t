-- Chooses between two values with a short-form boolean `on` expression.
enabled: bool = yes

main :: fn () => on enabled => 42 else 7
¬
42
¬

¬
global enabled
fn main
local $0 = i32:0
branch.false bool:enabled, L2
$0 = i32:42
jump L1
label L2
$0 = i32:7
label L1
return i32:$0
end
init
enabled = bool:yes
end
¬
bool $enabled;
int $main() {
    int $0 = 0;
    if (!$enabled) goto L2;
    $0 = 42;
    goto L1;
    L2: ;
    $0 = 7;
    L1: ;
    return $0;
}
void init() {
    $enabled = true;
}
