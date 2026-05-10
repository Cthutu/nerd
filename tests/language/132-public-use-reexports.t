use test.folder_pub_use
folder :: use test.folder_pub_use

main :: fn () -> i32 {
    return child_answer() + folder.local_answer()
}
¬
42
¬

¬
hir 0
module module.0(132-public-use-reexports.input)
import module.1(test.folder_pub_use)
import import.0 local_answer from module.1(test.folder_pub_use).decl.0: fn () -> i32
import import.1 child_answer from module.1(test.folder_pub_use).decl.1: fn () -> i32
bind local_answer = import.0
bind child_answer = import.1
bind folder = module.1
bind main = fn.0
func fn.0() -> i32 {
  return i32 add(i32 call bind.1(child_answer)(), i32 call fn () -> i32 field(module bind.2(folder), local_answer)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$local_answer()
declare i32 @$child_answer()

define i32 @fn.0() {
  %t0 = call i32 @$child_answer()
  %t1 = call i32 @$local_answer()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
