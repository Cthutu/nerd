use test.parts

main :: fn () -> i32 {
    return part_answer()
}
¬
42
¬

¬
hir 0
module module.0(135-module-parts.input)
import module.1(test.parts)
import import.0 Thing from module.1(test.parts).decl.0: Thing
import import.1 make_thing from module.1(test.parts).decl.1: fn (i32) -> Thing
import import.2 part_answer from module.1(test.parts).decl.2: fn () -> i32
bind Thing = import.0
bind make_thing = import.1
bind part_answer = import.2
bind main = fn.0
bind Thing = type.0
type type.0 = Thing
func fn.0() -> i32 {
  return i32 call bind.2(part_answer)()
}
¬
; nerd llvm-ir 0
; generated from HIR

declare { i32 } @$make_thing(i32)
declare i32 @$part_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @$part_answer()
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
