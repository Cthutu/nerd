wrapper :: use test.reexport

main :: fn() {
    wrapper.printer.prn("reexport")
    return 0
}
¬
0
¬
reexport

¬
fn main
call fn(string)->void:prn, string:"reexport"
return i32:0
end
¬
void init() {}
int $main() {
    prn((string){.data = (u8*)"reexport", .count = 8});
    return 0;
}
