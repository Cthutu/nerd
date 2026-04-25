use std.print

-- Shows that nested block state is dropped after leaving the block.
main :: fn () {
    value := 10
    {
        value := 32
        value = value + 1
        prn($"inner={value}")
    }
    prn($"outer={value}")
    return value
}
¬
10
¬
inner=33
outer=10

¬
fn main
string.reset
local value = i32:10
block
local value = i32:32
$0 = i32:value + i32:1
value = i32:$0
$1 = string.start
string.append string:"inner="
string.append i32:value
$2 = string.finish $1
call fn(string)->void:prn, string:$2
string.reset
end
$3 = string.start
string.append string:"outer="
string.append i32:value
$4 = string.finish $3
call fn(string)->void:prn, string:$4
string.reset
return i32:value
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $value = 10;
    {
        int $value = 32;
        int $0 = $value + 1;
        $value = $0;
        size_t $1 = string_builder_mark();
        string_builder_append_string(to_string$string((string){.data = (u8*)"inner=", .count = 6}));
        string_builder_append_string(to_string$i32($value));
        string $2 = string_builder_finish($1);
        prn($2);
        string_builder_reset();
    }
    size_t $3 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"outer=", .count = 6}));
    string_builder_append_string(to_string$i32($value));
    string $4 = string_builder_finish($3);
    prn($4);
    string_builder_reset();
    return $value;
}
