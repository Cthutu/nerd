name: string = "world"
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
init
name = "world"
count = 3
end
fn main
string.reset
$0 = string.start
string.append "Hello, ", type=2
string.append name, type=2
string.append "! count=", type=2
string.append count, type=3
$1 = string.finish $0
call prn, $1
string.reset
return 0
end
¬
string $name;
int $count;
void init() {
    $name = (string){.data = (u8*)"world", .count = 5};
    $count = 3;
}
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
