wrapper :: use test.reexport

main :: fn() {
    prn($"{wrapper.memory.kb(1)}")
    return 0
}
¬
0
¬
1024

¬
hir 0
bind wrapper = module.1
bind main = fn.0
func fn.0() -> i32 {
¬
; nerd llvm-ir 0
define internal i32 @fn.0() {
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
