-- Checks assertion messages can be interpolated strings.
main :: fn () -> i32 {
    count := 3
    assert count == 3, $"count is {count}"
    return 0
}
¬
0
¬
¬
fn main
string.reset
local count = i32:3
$0 = i32:count == i32:3
$1 = string.start
string.append string:"count is "
string.append i32:count
$2 = string.finish $1
assert bool:$0, string:$2, line 4
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    int $count = 3;
    bool $0 = $count == 3;
    size_t $1 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"count is ", .count = 9}));
    string_builder_append_string(to_string$i32($count));
    string $2 = string_builder_finish($1);
    nerd_assert($0, "tests/language/136-interpolated-assert-messages.t", 4, $2);
    return 0;
}
