use mod std.print

main :: fn() {
    prn("top")

    p :: mod std.print
    use p

    prn("local")
    pr("same")
    prn(" line")
}
¬
0
¬
top
local
same line

¬
fn main
call fn(string)->void:prn, string:"top"
call fn(string)->void:prn, string:"local"
call fn(string)->void:pr, string:"same"
call fn(string)->void:prn, string:" line"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"top", .count = 3});
    prn((string){.data = (u8*)"local", .count = 5});
    pr((string){.data = (u8*)"same", .count = 4});
    prn((string){.data = (u8*)" line", .count = 5});
    return 0;
}
