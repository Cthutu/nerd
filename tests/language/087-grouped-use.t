use test { grouped { alpha beta } }

main :: fn() -> i32 {
    return alpha() + beta()
}
¬
42
¬

¬
hir 0
module module.0(087-grouped-use.input)
import module.1(test.grouped.alpha)
import module.2(test.grouped.beta)
import import.0 alpha from module.1(test.grouped.alpha).decl.0: fn () -> i32
import import.1 beta from module.2(test.grouped.beta).decl.0: fn () -> i32
bind alpha = import.0
bind beta = import.1
bind main = fn.0
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(alpha)(), i32 call bind.1(beta)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$alpha()
declare i32 @$beta()

define internal i32 @fn.0() {
  %t0 = call i32 @$alpha()
  %t1 = call i32 @$beta()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
