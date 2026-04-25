use std.print

main :: fn () {
    total := 0
    found :: for i := 0; i < 1; i += 1 {
        total += i
        break total + 7
    } else {
        break -1
    }
    missing :: for j := 0; j < 0; j += 1 {
        break j
    } else {
        break 42
    }

    prn($"found = {found}")
    prn($"missing = {missing}")
    return found + missing
}
¬
49
¬
found = 7
missing = 42

¬
fn main
string.reset
local total = i32:0
local $0 = i32:0
local i = i32:0
label L1
$5 = i32:i < i32:1
branch.false bool:$5, L4
block
$6 = i32:total + i32:i
total = i32:$6
$7 = i32:total + i32:7
$0 = i32:$7
jump L3
end
label L2
$8 = i32:i + i32:1
i = i32:$8
jump L1
label L4
block
$0 = i32:-1
jump L3
end
label L3
local $9 = i32:0
local j = i32:0
label L10
$14 = i32:j < i32:0
branch.false bool:$14, L13
block
$9 = i32:j
jump L12
end
label L11
$15 = i32:j + i32:1
j = i32:$15
jump L10
label L13
block
$9 = i32:42
jump L12
end
label L12
$16 = string.start
string.append string:"found = "
string.append i32:$0
$17 = string.finish $16
call fn(string)->void:prn, string:$17
string.reset
$18 = string.start
string.append string:"missing = "
string.append i32:$9
$19 = string.finish $18
call fn(string)->void:prn, string:$19
string.reset
$20 = i32:$0 + i32:$9
return i32:$20
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $total = 0;
    int $0 = 0;
    int $i = 0;
    L1: ;
    bool $5 = $i < 1;
    if (!$5) goto L4;
    {
        int $6 = $total + $i;
        $total = $6;
        int $7 = $total + 7;
        $0 = $7;
        goto L3;
    }
    L2: ;
    int $8 = $i + 1;
    $i = $8;
    goto L1;
    L4: ;
    {
        $0 = -1;
        goto L3;
    }
    L3: ;
    int $9 = 0;
    int $j = 0;
    L10: ;
    bool $14 = $j < 0;
    if (!$14) goto L13;
    {
        $9 = $j;
        goto L12;
    }
    L11: ;
    int $15 = $j + 1;
    $j = $15;
    goto L10;
    L13: ;
    {
        $9 = 42;
        goto L12;
    }
    L12: ;
    size_t $16 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"found = ", .count = 8}));
    string_builder_append_string(to_string$i32($0));
    string $17 = string_builder_finish($16);
    prn($17);
    string_builder_reset();
    size_t $18 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"missing = ", .count = 10}));
    string_builder_append_string(to_string$i32($9));
    string $19 = string_builder_finish($18);
    prn($19);
    string_builder_reset();
    int $20 = $0 + $9;
    return $20;
}
