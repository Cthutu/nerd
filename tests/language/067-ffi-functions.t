abs :: ffi "c" (i32) -> i32

main :: fn() {
    value := abs(-7)
    prn($"abs = {value}")
}
¬
0
¬
abs = 7

¬
fn main
string.reset
$0 = call fn(i32)->i32:abs, i32:-7
local value = i32:$0
$1 = string.start
string.append string:"abs = "
string.append i32:value
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
return i32:0
end
¬
void init() {}
int abs(int);

int $main() {
    string_builder_reset();
    int $0 = abs(-7);
    int $value = $0;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"abs = ", .count = 6}));
    string_builder_append_string(to_string$i32($value));
    string $2 = string_builder_finish($1);
    prn($2);
    string_builder_reset();
    return 0;
}
