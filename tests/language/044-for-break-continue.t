main :: fn () {
    total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => continue
        on i == 5 => break
        total += i
    }
    return total
}
¬
8
¬
¬
fn main
local total = i32:0
local i = i32:0
label L0
$3 = i32:i < i32:6
branch.false bool:$3, L2
block
$4 = i32:i == i32:2
branch.false bool:$4, L5
jump L1
label L5
$6 = i32:i == i32:5
branch.false bool:$6, L7
jump L2
label L7
$8 = i32:total + i32:i
total = i32:$8
end
label L1
$9 = i32:i + i32:1
i = i32:$9
jump L0
label L2
return i32:total
end
¬
void init() {}
int $main() {
    int $total = 0;
    int $i = 0;
    L0: ;
    bool $3 = $i < 6;
    if (!$3) goto L2;
    {
        bool $4 = $i == 2;
        if (!$4) goto L5;
        goto L1;
        L5: ;
        bool $6 = $i == 5;
        if (!$6) goto L7;
        goto L2;
        L7: ;
        int $8 = $total + $i;
        $total = $8;
    }
    L1: ;
    int $9 = $i + 1;
    $i = $9;
    goto L0;
    L2: ;
    return $total;
}
