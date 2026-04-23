-- Matches integer values with block-form `on` branches.
size :: 2

main :: fn () => on size {
    0 => 10
    1 => 20
    else => 30
}
¬
30
¬

¬
fn main
local $0 = i32:0
$3 = i32:2 == i32:0
branch.false bool:$3, L2
$0 = i32:10
jump L1
label L2
$5 = i32:2 == i32:1
branch.false bool:$5, L4
$0 = i32:20
jump L1
label L4
$0 = i32:30
label L1
return i32:$0
end
¬
void init() {}
int $main() {
    int $0 = 0;
    bool $3 = 2 == 0;
    if (!$3) goto L2;
    $0 = 10;
    goto L1;
    L2: ;
    bool $5 = 2 == 1;
    if (!$5) goto L4;
    $0 = 20;
    goto L1;
    L4: ;
    $0 = 30;
    L1: ;
    return $0;
}
