on debug {
    use std.io
    answer :: 7

    on !feature {
        fallback :: 9
    }
}

main :: fn () {
    prn($"answer={answer}")
    prn($"fallback={fallback}")
}
¬
0
¬
answer=7
fallback=9

¬
fn main
string.reset
$0 = string.start
string.append string:"answer="
string.append i32:7
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$2 = string.start
string.append string:"fallback="
string.append i32:9
$3 = string.finish $2
call fn(string)->void:prn, string:$3
string.reset
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    size_t $0 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"answer=", .count = 7}));
    string_builder_append_string(to_string$i32(7));
    string $1 = string_builder_finish($0);
    prn($1);
    string_builder_reset();
    size_t $2 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"fallback=", .count = 9}));
    string_builder_append_string(to_string$i32(9));
    string $3 = string_builder_finish($2);
    prn($3);
    string_builder_reset();
    return 0;
}
