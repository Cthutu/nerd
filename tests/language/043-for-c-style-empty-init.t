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
$2 = i32:i < i32:3
branch.false bool:$2, L1
block
$3 = string.start
string.append string:"N "
string.append i32:i
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
end
$5 = i32:i + i32:1
i = i32:$5
jump L0
label L1
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $i = 0;
    L0: ;
    bool $2 = $i < 3;
    if (!$2) goto L1;
    {
        size_t $3 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"N ", .count = 2}));
        string_builder_append_string(to_string$i32($i));
        string $4 = string_builder_finish($3);
        prn($4);
        string_builder_reset();
    }
    int $5 = $i + 1;
    $i = $5;
    goto L0;
    L1: ;
    return 0;
}
