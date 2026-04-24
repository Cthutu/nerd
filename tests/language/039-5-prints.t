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
$1 = i32:5 <= i32:count
branch.false bool:$1, L2
return i32:0
label L2
$3 = string.start
string.append string:"Iteration "
string.append i32:count
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
$5 = i32:count + i32:1
count = i32:$5
end
jump L0
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $count = 0;
    L0: ;
    {
        bool $1 = 5 <= $count;
        if (!$1) goto L2;
        return 0;
        L2: ;
        size_t $3 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"Iteration ", .count = 10}));
        string_builder_append_string(to_string$i32($count));
        string $4 = string_builder_finish($3);
        prn($4);
        string_builder_reset();
        int $5 = $count + 1;
        $count = $5;
    }
    goto L0;
    return 0;
}
