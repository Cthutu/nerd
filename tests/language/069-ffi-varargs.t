use std.print

ffi "c" fcntl (i32, i32, ...) -> i32

main :: fn() {
    result := fcntl(0, 1, 0)
    prn($"fcntl = {result >= 0}")
}
¬
0
¬
fcntl = yes

¬
fn main
string.reset
$0 = call fn(i32,i32,...)->i32:fcntl, i32:0, i32:1, i32:0
local result = i32:$0
$1 = string.start
string.append string:"fcntl = "
$3 = i32:0 <= i32:result
string.append bool:$3
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
return i32:0
end
¬
void init() {}
int fcntl(int, int, ...);

int $main() {
    string_builder_reset();
    int $0 = fcntl(0, 1, 0);
    int $result = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"fcntl = ", .count = 8}));
    bool $3 = 0 <= $result;
    string_builder_append_string(to_string$bool($3));
    string $2 = string_builder_finish($1);
    prn($2);
    string_builder_reset();
    return 0;
}
