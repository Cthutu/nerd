use mod std.print

main :: fn() {
    p :: mod std.print
    use p

    prn("Hello")
    pr("same")
    prn(" line")
}
¬
0
¬
Hello
same line

¬
fn main
call fn(string)->void:prn, string:"Hello"
call fn(string)->void:pr, string:"same"
call fn(string)->void:prn, string:" line"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Hello", .count = 5});
    pr((string){.data = (u8*)"same", .count = 4});
    prn((string){.data = (u8*)" line", .count = 5});
    return 0;
}
