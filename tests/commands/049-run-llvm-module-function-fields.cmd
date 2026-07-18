memory :: use std.mem

main :: fn() {
    local_memory :: use std.mem

    _ := local_memory.kb(1)
    _ := memory.mb(1)
    prn("Local")
    prn("Hello")
    pr("same")
    prn(" line")
}
¬
0
¬
Local
Hello
same line

¬
delete
¬
