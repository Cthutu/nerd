use std.io

main :: fn () {
    i := 0
    for i < 5 {
        prn($"While {i}")
        i += 1
    }
}
¬
0
¬
While 0
While 1
While 2
While 3
While 4

¬
fn main
string.reset
local i = i32:0
label L0
$2 = i32:i < i32:5
branch.false bool:$2, L1
block
$3 = string.start
string.append string:"While "
string.append i32:i
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$5 = i32:i + i32:1
i = i32:$5
end
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
    bool $2 = $i < 5;
    if (!$2) goto L1;
    {
        size_t $3 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"While ", .count = 6}));
        string_builder_append_string(to_string$i32($i));
        string $4 = string_builder_finish($3);
        prn($4);
        string_builder_reset();
        int $5 = $i + 1;
        $i = $5;
    }
    goto L0;
    L1: ;
    return 0;
}
