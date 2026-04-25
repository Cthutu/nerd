print :: mod std.print

main :: fn() {
    local_print :: mod std.print

    local_print.prn("Local")
    print.prn("Hello")
    print.pr("same")
    print.prn(" line")
}
¬
0
¬
Local
Hello
same line

¬
fn main
call fn(string)->void:prn, string:"Local"
call fn(string)->void:prn, string:"Hello"
call fn(string)->void:pr, string:"same"
call fn(string)->void:prn, string:" line"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"Local", .count = 5});
    prn((string){.data = (u8*)"Hello", .count = 5});
    pr((string){.data = (u8*)"same", .count = 4});
    prn((string){.data = (u8*)" line", .count = 5});
    return 0;
}
