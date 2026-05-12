use test.local_bare_module

main :: fn () -> i32 {
    return local_bare_module_answer()
}
¬
17
¬

¬
hir 0
module module.0(152-local-bare-module-use.input)
import module.1(test.local_bare_module)
import import.0 local_bare_module_answer from module.1(test.local_bare_module).decl.0: fn () -> i32
bind local_bare_module_answer = import.0
bind main = fn.0
func fn.0() -> i32 {
  return i32 call bind.0(local_bare_module_answer)()
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$local_bare_module_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @$local_bare_module_answer()
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
