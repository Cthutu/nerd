use std.io

-- Checks float literals plus comparison, logical, bitwise, and modulo operators.
half: f32 = 0.5
limit: f64 = 2.0

main :: fn () {
    mask: u32 = 14
    ordered := 3 < 4 && 4 <= 4 && 5 > 4 && 5 >= 5
    prn(on (1.5 < limit && !no) => "float" else "bad")
    prn(on ordered => "cmp" else "bad")
    prn(on (5 % 2 == 1) => "mod" else "bad")
    prn(on (((mask & 11) ^ 3) == 9 || no) => "bits" else "bad")
    return on (half <= 0.5 && 3 != 4) => 1 else 0
}
¬
1
¬
float
cmp
mod
bits

¬
global half
global limit
fn main
local mask = u32:14
local $0 = bool:no
local $3 = bool:no
local $6 = bool:no
$9 = i32:3 < i32:4
branch.false bool:$9, L7
$10 = i32:4 <= i32:4
branch.false bool:$10, L7
$6 = bool:yes
jump L8
label L7
$6 = bool:no
label L8
branch.false bool:$6, L4
$11 = i32:4 < i32:5
branch.false bool:$11, L4
$3 = bool:yes
jump L5
label L4
$3 = bool:no
label L5
branch.false bool:$3, L1
$12 = i32:5 <= i32:5
branch.false bool:$12, L1
$0 = bool:yes
jump L2
label L1
$0 = bool:no
label L2
local ordered = bool:$0
local $13 = bool:no
$16 = f64:1.5 < f64:limit
branch.false bool:$16, L14
$17 = !bool:no
branch.false bool:$17, L14
$13 = bool:yes
jump L15
label L14
$13 = bool:no
label L15
local $18 = string:0
branch.false bool:$13, L20
$18 = string:"float"
jump L19
label L20
$18 = string:"bad"
label L19
call fn(string)->void:prn, string:$18
local $21 = string:0
branch.false bool:ordered, L23
$21 = string:"cmp"
jump L22
label L23
$21 = string:"bad"
label L22
call fn(string)->void:prn, string:$21
$24 = i32:1 == i32:1
local $25 = string:0
branch.false bool:$24, L27
$25 = string:"mod"
jump L26
label L27
$25 = string:"bad"
label L26
call fn(string)->void:prn, string:$25
local $28 = bool:no
$32 = u32:mask & u32:11
$33 = u32:$32 ^ u32:3
$34 = u32:$33 == u32:9
branch.false bool:$34, L29
$28 = bool:yes
jump L31
label L29
branch.false bool:no, L30
$28 = bool:yes
jump L31
label L30
$28 = bool:no
label L31
local $35 = string:0
branch.false bool:$28, L37
$35 = string:"bits"
jump L36
label L37
$35 = string:"bad"
label L36
call fn(string)->void:prn, string:$35
local $38 = bool:no
$41 = f32:half <= f32:0.5
branch.false bool:$41, L39
$42 = i32:3 != i32:4
branch.false bool:$42, L39
$38 = bool:yes
jump L40
label L39
$38 = bool:no
label L40
local $43 = i32:0
branch.false bool:$38, L45
$43 = i32:1
jump L44
label L45
$43 = i32:0
label L44
return i32:$43
end
init
half = f32:0.5
limit = f64:2.0
end
¬
float $half;
double $limit;
int $main() {
    uint32_t $mask = 14;
    bool $0 = false;
    bool $3 = false;
    bool $6 = false;
    bool $9 = 3 < 4;
    if (!$9) goto L7;
    bool $10 = 4 <= 4;
    if (!$10) goto L7;
    $6 = true;
    goto L8;
    L7: ;
    $6 = false;
    L8: ;
    if (!$6) goto L4;
    bool $11 = 4 < 5;
    if (!$11) goto L4;
    $3 = true;
    goto L5;
    L4: ;
    $3 = false;
    L5: ;
    if (!$3) goto L1;
    bool $12 = 5 <= 5;
    if (!$12) goto L1;
    $0 = true;
    goto L2;
    L1: ;
    $0 = false;
    L2: ;
    bool $ordered = $0;
    bool $13 = false;
    bool $16 = 1.5 < $limit;
    if (!$16) goto L14;
    bool $17 = !0;
    if (!$17) goto L14;
    $13 = true;
    goto L15;
    L14: ;
    $13 = false;
    L15: ;
    string $18 = (string){0};
    if (!$13) goto L20;
    $18 = (string){.data = (u8*)"float", .count = 5};
    goto L19;
    L20: ;
    $18 = (string){.data = (u8*)"bad", .count = 3};
    L19: ;
    prn($18);
    string $21 = (string){0};
    if (!$ordered) goto L23;
    $21 = (string){.data = (u8*)"cmp", .count = 3};
    goto L22;
    L23: ;
    $21 = (string){.data = (u8*)"bad", .count = 3};
    L22: ;
    prn($21);
    bool $24 = 1 == 1;
    string $25 = (string){0};
    if (!$24) goto L27;
    $25 = (string){.data = (u8*)"mod", .count = 3};
    goto L26;
    L27: ;
    $25 = (string){.data = (u8*)"bad", .count = 3};
    L26: ;
    prn($25);
    bool $28 = false;
    uint32_t $32 = $mask & 11;
    uint32_t $33 = $32 ^ 3;
    bool $34 = $33 == 9;
    if (!$34) goto L29;
    $28 = true;
    goto L31;
    L29: ;
    if (!0) goto L30;
    $28 = true;
    goto L31;
    L30: ;
    $28 = false;
    L31: ;
    string $35 = (string){0};
    if (!$28) goto L37;
    $35 = (string){.data = (u8*)"bits", .count = 4};
    goto L36;
    L37: ;
    $35 = (string){.data = (u8*)"bad", .count = 3};
    L36: ;
    prn($35);
    bool $38 = false;
    bool $41 = $half <= 0.5f;
    if (!$41) goto L39;
    bool $42 = 3 != 4;
    if (!$42) goto L39;
    $38 = true;
    goto L40;
    L39: ;
    $38 = false;
    L40: ;
    int $43 = 0;
    if (!$38) goto L45;
    $43 = 1;
    goto L44;
    L45: ;
    $43 = 0;
    L44: ;
    return $43;
}
void init() {
    $half = 0.5f;
    $limit = 2.0;
}
