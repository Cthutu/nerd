main :: fn () {
    n := 1
    n += 2
    n *= 4
    n -= 5
    n /= 2
    n %= 2

    bits := 6
    bits &= 3
    bits ^= 7
    bits |= 8

    flag := true
    flag &&= false
    flag ||= true

    prn($"{n} {bits} {flag}")
}
¬
0
¬
1 13 true

¬
fn main
string.reset
local n = i32:1
$0 = i32:n + i32:2
n = i32:$0
$1 = i32:n * i32:4
n = i32:$1
$2 = i32:n - i32:5
n = i32:$2
$3 = i32:n / i32:2
n = i32:$3
$4 = i32:n % i32:2
n = i32:$4
local bits = i32:6
$5 = i32:bits & i32:3
bits = i32:$5
$6 = i32:bits ^ i32:7
bits = i32:$6
$7 = i32:bits | i32:8
bits = i32:$7
local flag = bool:true
local $8 = bool:false
branch.false bool:flag, L9
branch.false bool:false, L9
$8 = bool:true
jump L10
label L9
$8 = bool:false
label L10
flag = bool:$8
local $11 = bool:false
branch.false bool:flag, L12
$11 = bool:true
jump L14
label L12
branch.false bool:true, L13
$11 = bool:true
jump L14
label L13
$11 = bool:false
label L14
flag = bool:$11
$15 = string.start
string.append i32:n
string.append string:" "
string.append i32:bits
string.append string:" "
string.append bool:flag
$16 = string.finish $15
call fn(string)->void:prn, string:$16
string.reset
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $n = 1;
    int $0 = $n + 2;
    $n = $0;
    int $1 = $n * 4;
    $n = $1;
    int $2 = $n - 5;
    $n = $2;
    int $3 = $n / 2;
    $n = $3;
    int $4 = $n % 2;
    $n = $4;
    int $bits = 6;
    int $5 = $bits & 3;
    $bits = $5;
    int $6 = $bits ^ 7;
    $bits = $6;
    int $7 = $bits | 8;
    $bits = $7;
    bool $flag = true;
    bool $8 = false;
    if (!$flag) goto L9;
    if (!0) goto L9;
    $8 = true;
    goto L10;
    L9: ;
    $8 = false;
    L10: ;
    $flag = $8;
    bool $11 = false;
    if (!$flag) goto L12;
    $11 = true;
    goto L14;
    L12: ;
    if (!1) goto L13;
    $11 = true;
    goto L14;
    L13: ;
    $11 = false;
    L14: ;
    $flag = $11;
    size_t $15 = string_builder_mark();
    string_builder_append_string(to_string$i32($n));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$i32($bits));
    string_builder_append_string(to_string$string((string){.data = (u8*)" ", .count = 1}));
    string_builder_append_string(to_string$bool($flag));
    string $16 = string_builder_finish($15);
    prn($16);
    string_builder_reset();
    return 0;
}
