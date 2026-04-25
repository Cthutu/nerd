use std.print

main :: fn () {
    i := 0
    for ; i < 3; i += 1 {
        prn($"N {i}")
    }
}
¬
0
¬
N 0
N 1
N 2

¬
fn main
string.reset
local i = i32:0
label L0
$3 = i32:i < i32:3
branch.false bool:$3, L2
block
$4 = string.start
string.append string:"N "
string.append i32:i
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
end
label L1
$6 = i32:i + i32:1
i = i32:$6
jump L0
label L2
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $i = 0;
    L0: ;
    bool $3 = $i < 3;
    if (!$3) goto L2;
    {
        size_t $4 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"N ", .count = 2}));
        string_builder_append_string(to_string$i32($i));
        string $5 = string_builder_finish($4);
        prn($5);
        string_builder_reset();
    }
    L1: ;
    int $6 = $i + 1;
    $i = $6;
    goto L0;
    L2: ;
    return 0;
}
