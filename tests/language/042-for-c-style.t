use mod std.print

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
$3 = i32:i < i32:5
branch.false bool:$3, L2
block
$4 = string.start
string.append string:"C "
string.append i32:i
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
end
label L1
$6 = i32:total + i32:step
total = i32:$6
$7 = i32:i + i32:1
i = i32:$7
jump L0
label L2
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
    bool $3 = $i < 5;
    if (!$3) goto L2;
    {
        size_t $4 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"C ", .count = 2}));
        string_builder_append_string(to_string$i32($i));
        string $5 = string_builder_finish($4);
        prn($5);
        string_builder_reset();
    }
    L1: ;
    int $6 = $total + $step;
    $total = $6;
    int $7 = $i + 1;
    $i = $7;
    goto L0;
    L2: ;
    return $total;
}
