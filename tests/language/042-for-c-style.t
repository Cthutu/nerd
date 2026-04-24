main :: fn () {
    total := 0
    for i := 0, step := 1; i < 5; total += step, i += 1 {
        prn($"C {i}")
    }
    return total
}
¬
5
¬
C 0
C 1
C 2
C 3
C 4

¬
fn main
string.reset
local total = i32:0
local i = i32:0
local step = i32:1
label L0
$2 = i32:i < i32:5
branch.false bool:$2, L1
block
$3 = string.start
string.append string:"C "
string.append i32:i
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
end
$5 = i32:total + i32:step
total = i32:$5
$6 = i32:i + i32:1
i = i32:$6
jump L0
label L1
return i32:total
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $total = 0;
    int $i = 0;
    int $step = 1;
    L0: ;
    bool $2 = $i < 5;
    if (!$2) goto L1;
    {
        size_t $3 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"C ", .count = 2}));
        string_builder_append_string(to_string$i32($i));
        string $4 = string_builder_finish($3);
        prn($4);
        string_builder_reset();
    }
    int $5 = $total + $step;
    $total = $5;
    int $6 = $i + 1;
    $i = $6;
    goto L0;
    L1: ;
    return $total;
}
