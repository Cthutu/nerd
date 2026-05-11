use test.folder_mod
priority :: use test.folder_priority

main :: fn () -> i32 {
    return answer() + priority.priority_answer()
}
¬
49
¬

¬
hir 0
module module.0(131-module-folder-imports.input)
import module.1(test.folder_mod)
import module.2(test.folder_priority)
import import.0 answer from module.1(test.folder_mod).decl.0: fn () -> i32
import import.1 label from module.1(test.folder_mod).decl.1: string
import import.2 priority_answer from module.2(test.folder_priority).decl.0: fn () -> i32
bind answer = import.0
bind label = import.1
bind priority_answer = import.2
bind priority = module.2
bind main = fn.0
bind label = value.0
const value.0: string
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(answer)(), i32 call fn () -> i32 field(module bind.3(priority), priority_answer)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$answer()
declare i32 @$priority_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @$answer()
  %t1 = call i32 @$priority_answer()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
