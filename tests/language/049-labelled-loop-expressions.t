use std.print

main :: fn () {
    total := 0
    for i := 0; i < 5; i += 1 $outer {
        for j := 0; j < 5; j += 1 $inner {
            on j == 1 => continue $outer
            total += 100
        }
        total += i
    }

    for $done {
        break $done
    }

    value :: for {
        break 7
    }
    labelled_value :: for $value_loop {
        break $value_loop 11
    }

    prn($"total = {total}")
    prn($"value = {value}")
    prn($"labelled = {labelled_value}")
    return total + value + labelled_value
}
¬
6
¬
total = 500
value = 7
labelled = 11

¬
fn main
string.reset
local total = i32:0
local i = i32:0
label L0
$3 = i32:i < i32:5
branch.false bool:$3, L2
block
local j = i32:0
label L4
$7 = i32:j < i32:5
branch.false bool:$7, L6
block
$8 = i32:j == i32:1
branch.false bool:$8, L9
jump L1
label L9
$10 = i32:total + i32:100
total = i32:$10
end
label L5
$11 = i32:j + i32:1
j = i32:$11
jump L4
label L6
$12 = i32:total + i32:i
total = i32:$12
end
label L1
$13 = i32:i + i32:1
i = i32:$13
jump L0
label L2
label L14
block
jump L15
end
jump L14
label L15
local $16 = i32:0
label L17
block
$16 = i32:7
jump L18
end
jump L17
label L18
local $19 = i32:0
label L20
block
$19 = i32:11
jump L21
end
jump L20
label L21
$22 = string.start
string.append string:"total = "
string.append i32:total
$23 = string.finish $22
call fn(string)->void:prn, string:$23
string.reset
$24 = string.start
string.append string:"value = "
string.append i32:$16
$25 = string.finish $24
call fn(string)->void:prn, string:$25
string.reset
$26 = string.start
string.append string:"labelled = "
string.append i32:$19
$27 = string.finish $26
call fn(string)->void:prn, string:$27
string.reset
$28 = i32:total + i32:$16
$29 = i32:$28 + i32:$19
return i32:$29
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $total = 0;
    int $i = 0;
    L0: ;
    bool $3 = $i < 5;
    if (!$3) goto L2;
    {
        int $j = 0;
        L4: ;
        bool $7 = $j < 5;
        if (!$7) goto L6;
        {
            bool $8 = $j == 1;
            if (!$8) goto L9;
            goto L1;
            L9: ;
            int $10 = $total + 100;
            $total = $10;
        }
        L5: ;
        int $11 = $j + 1;
        $j = $11;
        goto L4;
        L6: ;
        int $12 = $total + $i;
        $total = $12;
    }
    L1: ;
    int $13 = $i + 1;
    $i = $13;
    goto L0;
    L2: ;
    L14: ;
    {
        goto L15;
    }
    goto L14;
    L15: ;
    int $16 = 0;
    L17: ;
    {
        $16 = 7;
        goto L18;
    }
    goto L17;
    L18: ;
    int $19 = 0;
    L20: ;
    {
        $19 = 11;
        goto L21;
    }
    goto L20;
    L21: ;
    size_t $22 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"total = ", .count = 8}));
    string_builder_append_string(to_string$i32($total));
    string $23 = string_builder_finish($22);
    prn($23);
    string_builder_reset();
    size_t $24 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"value = ", .count = 8}));
    string_builder_append_string(to_string$i32($16));
    string $25 = string_builder_finish($24);
    prn($25);
    string_builder_reset();
    size_t $26 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"labelled = ", .count = 11}));
    string_builder_append_string(to_string$i32($19));
    string $27 = string_builder_finish($26);
    prn($27);
    string_builder_reset();
    int $28 = $total + $16;
    int $29 = $28 + $19;
    return $29;
}
