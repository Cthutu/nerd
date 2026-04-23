-- Builds an interpolated string from a constant and a variable.
name :: "world"
count := 3

main :: fn () {
    prn($"Hello, {name}! count={count}")
}
¬
0
¬
Hello, world! count=3

¬
global name
global count
fn main
string.reset
$0 = string.start
string.append string:"Hello, "
string.append string:name
string.append string:"! count="
string.append i32:count
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
return i32:0
end
init
name = string:"world"
count = i32:3
end
¬
string $name;
int $count;
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"Hello, ", .count = 7}));
    string_builder_append_string(to_string$string($name));
    string_builder_append_string(to_string$string((string){.data = (u8*)"! count=", .count = 8}));
    string_builder_append_string(to_string$i32($count));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    return 0;
}
void init() {
    $name = (string){.data = (u8*)"world", .count = 5};
    $count = 3;
}
