use std.print

main :: fn () {
    count := 0

    for {
        on count >= 5 => return

        prn($"Iteration {count}")
        count += 1
    }
}
¬
0
¬
Iteration 0
Iteration 1
Iteration 2
Iteration 3
Iteration 4

¬
fn main
string.reset
local count = i32:0
label L0
block
$2 = i32:5 <= i32:count
branch.false bool:$2, L3
return i32:0
label L3
$4 = string.start
string.append string:"Iteration "
string.append i32:count
$5 = string.finish $4
call fn(string)->void:prn, string:$5
string.reset
$6 = i32:count + i32:1
count = i32:$6
end
jump L0
label L1
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $count = 0;
    L0: ;
    {
        bool $2 = 5 <= $count;
        if (!$2) goto L3;
        return 0;
        L3: ;
        size_t $4 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"Iteration ", .count = 10}));
        string_builder_append_string(to_string$i32($count));
        string $5 = string_builder_finish($4);
        prn($5);
        string_builder_reset();
        int $6 = $count + 1;
        $count = $6;
    }
    goto L0;
    L1: ;
    return 0;
}
