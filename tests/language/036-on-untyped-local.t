use mod std.print

-- Block-form `on` materialises untyped local integer constants to i32.
main :: fn () {
    i :: 2

    prn(on i {
        0 => "zero"
        else as x => $"non-zero: {x}"
    })
}
¬
0
¬
non-zero: 2

¬
fn main
string.reset
local $0 = string:0
$4 = i32:2 == i32:0
branch.false bool:$4, L2
label L3
$0 = string:"zero"
jump L1
label L2
$5 = string.start
string.append string:"non-zero: "
string.append i32:2
$6 = string.finish $5
$0 = string:$6
label L1
call fn(string)->void:prn, string:$0
string.reset
return i32:0
end
¬
void init() {}
int $main() {
    string_builder_reset();
    string $0 = (string){0};
    bool $4 = 2 == 0;
    if (!$4) goto L2;
    L3: ;
    $0 = (string){.data = (u8*)"zero", .count = 4};
    goto L1;
    L2: ;
    size_t $5 = string_builder_mark();
    string_builder_append_string(to_string$string((string){.data = (u8*)"non-zero: ", .count = 10}));
    string_builder_append_string(to_string$i32(2));
    string $6 = string_builder_finish($5);
    $0 = $6;
    L1: ;
    prn($0);
    string_builder_reset();
    return 0;
}
