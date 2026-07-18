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
hir 0
bind memory = module.1
bind main = fn.0
func fn.0() -> void {
¬
; nerd llvm-ir 0
define internal void @fn.0() {
  ret void
}

@$main = alias void (), ptr @fn.0
