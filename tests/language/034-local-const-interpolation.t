-- Interpolates block-local constant bindings, including an unannotated float.
main :: fn () {
    i :: 2
    f :: 3.14
    s :: "Hello, world!"

    prn($"{s}  i = {i} and f = {f}!")
}
¬
0
¬
Hello, world!  i = 2 and f = 3.14!

¬
fn main
string.reset
$0 = string.start
string.append string:"Hello, world!"
string.append string:"  i = "
string.append i32:2
string.append string:" and f = "
string.append f64:3.1400000000000001
string.append string:"!"
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"Hello, world!", .count = 13}));
    string_builder_append_string(to_string$string((string){.data = (u8*)"  i = ", .count = 6}));
    string_builder_append_string(to_string$i32(2));
    string_builder_append_string(to_string$string((string){.data = (u8*)" and f = ", .count = 9}));
    string_builder_append_string(to_string$f64(3.1400000000000001));
    string_builder_append_string(to_string$string((string){.data = (u8*)"!", .count = 1}));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    return 0;
}
