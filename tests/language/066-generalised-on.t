score :: fn (value: i32) -> i32 {
    return on value {
        < 0 => -1
        == 0 => 0
        1, 2 => 10
        >= 10 => 100
        else => 20
    }
}

describe :: fn (value: i32) -> string {
    return on {
        value < 0 => "negative"
        value == 0 => "zero"
        value < 10 => "small"
        else => "large"
    }
}

not_five :: fn (value: i32) -> i32 {
    return on value {
        != 5 => 1
        else => 0
    }
}

main :: fn () -> i32 {
    prn($"scores {score(-2)} {score(0)} {score(2)} {score(7)} {score(12)}")
    prn($"descriptions {describe(-2)} {describe(0)} {describe(7)} {describe(12)}")
    prn($"not-five {not_five(4)} {not_five(5)}")
    return score(12)
}
¬
100
¬
scores -1 0 10 20 100
descriptions negative zero small large
not-five 1 0

¬
fn score
param i32:value
local $0 = i32:0
$4 = i32:value < i32:0
branch.false bool:$4, L2
label L3
$0 = i32:-1
jump L1
label L2
$7 = i32:value == i32:0
branch.false bool:$7, L5
label L6
$0 = i32:0
jump L1
label L5
$11 = i32:value == i32:1
branch.false bool:$11, L10
jump L9
label L10
$12 = i32:value == i32:2
branch.false bool:$12, L8
label L9
$0 = i32:10
jump L1
label L8
$15 = i32:10 <= i32:value
branch.false bool:$15, L13
label L14
$0 = i32:100
jump L1
label L13
$0 = i32:20
label L1
return i32:$0
end
fn describe
param i32:value
local $0 = string:0
$3 = i32:value < i32:0
branch.false bool:$3, L2
$0 = string:"negative"
jump L1
label L2
$5 = i32:value == i32:0
branch.false bool:$5, L4
$0 = string:"zero"
jump L1
label L4
$7 = i32:value < i32:10
branch.false bool:$7, L6
$0 = string:"small"
jump L1
label L6
$0 = string:"large"
label L1
return string:$0
end
fn not_five
param i32:value
local $0 = i32:0
$4 = i32:value != i32:5
branch.false bool:$4, L2
label L3
$0 = i32:1
jump L1
label L2
$0 = i32:0
label L1
return i32:$0
end
fn main
string.reset
$0 = string.start
string.append string:"scores "
$2 = call fn(i32)->i32:score, i32:-2
string.append i32:$2
string.append string:" "
$3 = call fn(i32)->i32:score, i32:0
string.append i32:$3
string.append string:" "
$4 = call fn(i32)->i32:score, i32:2
string.append i32:$4
string.append string:" "
$5 = call fn(i32)->i32:score, i32:7
string.append i32:$5
string.append string:" "
$6 = call fn(i32)->i32:score, i32:12
string.append i32:$6
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$7 = string.start
string.append string:"descriptions "
$9 = call fn(i32)->string:describe, i32:-2
string.append string:$9
string.append string:" "
$10 = call fn(i32)->string:describe, i32:0
string.append string:$10
string.append string:" "
$11 = call fn(i32)->string:describe, i32:7
string.append string:$11
string.append string:" "
$12 = call fn(i32)->string:describe, i32:12
string.append string:$12
$8 = string.finish $7
call fn(string)->void:prn, string:$8
string.reset
$13 = string.start
string.append string:"not-five "
$15 = call fn(i32)->i32:not_five, i32:4
string.append i32:$15
string.append string:" "
$16 = call fn(i32)->i32:not_five, i32:5
string.append i32:$16
$14 = string.finish $13
call fn(string)->void:prn, string:$14
string.reset
$17 = call fn(i32)->i32:score, i32:12
return i32:$17
end
¬
void init() {}
int $score(int $value) {
    int $0 = 0;
    bool $4 = $value < 0;
    if (!$4) goto L2;
    L3: ;
    $0 = -1;
    goto L1;
    L2: ;
    bool $7 = $value == 0;
    if (!$7) goto L5;
    L6: ;
    $0 = 0;
    goto L1;
    L5: ;
    bool $11 = $value == 1;
    if (!$11) goto L10;
    goto L9;
    L10: ;
    bool $12 = $value == 2;
    if (!$12) goto L8;
    L9: ;
    $0 = 10;
    goto L1;
    L8: ;
    bool $15 = 10 <= $value;
    if (!$15) goto L13;
    L14: ;
    $0 = 100;
    goto L1;
    L13: ;
    $0 = 20;
    L1: ;
    return $0;
}
string $describe(int $value) {
    string $0 = (string){0};
    bool $3 = $value < 0;
    if (!$3) goto L2;
    $0 = (string){.data = (u8*)"negative", .count = 8};
    goto L1;
    L2: ;
    bool $5 = $value == 0;
    if (!$5) goto L4;
    $0 = (string){.data = (u8*)"zero", .count = 4};
    goto L1;
    L4: ;
    bool $7 = $value < 10;
    if (!$7) goto L6;
    $0 = (string){.data = (u8*)"small", .count = 5};
    goto L1;
    L6: ;
    $0 = (string){.data = (u8*)"large", .count = 5};
    L1: ;
    return $0;
}
int $not_five(int $value) {
    int $0 = 0;
    bool $4 = $value != 5;
    if (!$4) goto L2;
    L3: ;
    $0 = 1;
    goto L1;
    L2: ;
    $0 = 0;
    L1: ;
    return $0;
}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"scores ", .count = 7}));
    int $2 = $score(-2);
    string_builder_append_string(to_string$i32($2));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $3 = $score(0);
    string_builder_append_string(to_string$i32($3));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $4 = $score(2);
    string_builder_append_string(to_string$i32($4));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $5 = $score(7);
    string_builder_append_string(to_string$i32($5));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $6 = $score(12);
    string_builder_append_string(to_string$i32($6));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $7 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"descriptions ", .count = 13}));
    string $9 = $describe(-2);
    string_builder_append_string(to_string$string($9));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $10 = $describe(0);
    string_builder_append_string(to_string$string($10));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $11 = $describe(7);
    string_builder_append_string(to_string$string($11));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string $12 = $describe(12);
    string_builder_append_string(to_string$string($12));
    string $8 = string_builder_finish($7);
    prn($8);
    string_builder_reset();
    size_t $13 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"not-five ", .count = 9}));
    int $15 = $not_five(4);
    string_builder_append_string(to_string$i32($15));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    int $16 = $not_five(5);
    string_builder_append_string(to_string$i32($16));
    string $14 = string_builder_finish($13);
    prn($14);
    string_builder_reset();
    int $17 = $score(12);
    return $17;
}
