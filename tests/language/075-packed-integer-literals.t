use std.print

a :: 'a'
ab :: 'ab'
abc :: 'abc'
abcde :: 'abcde'

main :: fn () -> i32 {
    ok := a == 97 && ab == 24930 && abc == 6382179 && abcde == 418262508645
    upper := on 'Q' {
        'A' ..= 'Z' => yes
        else => no
    }
    lower := on 'q' {
        'A' ..= 'Z' => yes
        else => no
    }

    return on ok && upper && !lower => 0 else 1
}
¬
0
¬
¬
fn main
local $0 = bool:no
local $3 = bool:no
local $6 = bool:no
$9 = u8:97 == u8:97
branch.false bool:$9, L7
$10 = u16:24930 == u16:24930
branch.false bool:$10, L7
$6 = bool:yes
jump L8
label L7
$6 = bool:no
label L8
branch.false bool:$6, L4
$11 = u32:6382179 == u32:6382179
branch.false bool:$11, L4
$3 = bool:yes
jump L5
label L4
$3 = bool:no
label L5
branch.false bool:$3, L1
$12 = u64:418262508645 == u64:418262508645
branch.false bool:$12, L1
$0 = bool:yes
jump L2
label L1
$0 = bool:no
label L2
local ok = bool:$0
local $13 = bool:no
$17 = u8:65 <= u8:81
branch.false bool:$17, L15
$18 = u8:81 <= u8:90
branch.false bool:$18, L15
label L16
$13 = bool:yes
jump L14
label L15
$13 = bool:no
label L14
local upper = bool:$13
local $19 = bool:no
$23 = u8:65 <= u8:113
branch.false bool:$23, L21
$24 = u8:113 <= u8:90
branch.false bool:$24, L21
label L22
$19 = bool:yes
jump L20
label L21
$19 = bool:no
label L20
local lower = bool:$19
local $25 = bool:no
local $28 = bool:no
branch.false bool:ok, L29
branch.false bool:upper, L29
$28 = bool:yes
jump L30
label L29
$28 = bool:no
label L30
branch.false bool:$28, L26
$31 = !bool:lower
branch.false bool:$31, L26
$25 = bool:yes
jump L27
label L26
$25 = bool:no
label L27
local $32 = i32:0
branch.false bool:$25, L34
$32 = i32:0
jump L33
label L34
$32 = i32:1
label L33
return i32:$32
end
¬
void init() {}
int $main() {
    bool $0 = false;
    bool $3 = false;
    bool $6 = false;
    bool $9 = 97 == 97;
    if (!$9) goto L7;
    bool $10 = 24930 == 24930;
    if (!$10) goto L7;
    $6 = true;
    goto L8;
    L7: ;
    $6 = false;
    L8: ;
    if (!$6) goto L4;
    bool $11 = 6382179 == 6382179;
    if (!$11) goto L4;
    $3 = true;
    goto L5;
    L4: ;
    $3 = false;
    L5: ;
    if (!$3) goto L1;
    bool $12 = 418262508645 == 418262508645;
    if (!$12) goto L1;
    $0 = true;
    goto L2;
    L1: ;
    $0 = false;
    L2: ;
    bool $ok = $0;
    bool $13 = false;
    bool $17 = 65 <= 81;
    if (!$17) goto L15;
    bool $18 = 81 <= 90;
    if (!$18) goto L15;
    L16: ;
    $13 = true;
    goto L14;
    L15: ;
    $13 = false;
    L14: ;
    bool $upper = $13;
    bool $19 = false;
    bool $23 = 65 <= 113;
    if (!$23) goto L21;
    bool $24 = 113 <= 90;
    if (!$24) goto L21;
    L22: ;
    $19 = true;
    goto L20;
    L21: ;
    $19 = false;
    L20: ;
    bool $lower = $19;
    bool $25 = false;
    bool $28 = false;
    if (!$ok) goto L29;
    if (!$upper) goto L29;
    $28 = true;
    goto L30;
    L29: ;
    $28 = false;
    L30: ;
    if (!$28) goto L26;
    bool $31 = !$lower;
    if (!$31) goto L26;
    $25 = true;
    goto L27;
    L26: ;
    $25 = false;
    L27: ;
    int $32 = 0;
    if (!$25) goto L34;
    $32 = 0;
    goto L33;
    L34: ;
    $32 = 1;
    L33: ;
    return $32;
}
