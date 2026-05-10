a :: use test.namespaced_a
b :: use test.namespaced_b

main :: fn () -> i32 {
    return a.test() + b.test()
}
¬
42
¬

¬
hir 0
module module.0(145-module-symbol-namespaces.input)
import module.1(test.namespaced_a)
import module.2(test.namespaced_b)
import import.0 test from module.1(test.namespaced_a).decl.0: fn () -> i32
import import.1 test from module.2(test.namespaced_b).decl.0: fn () -> i32
bind test = import.0
bind test = import.1
bind a = module.1
bind b = module.2
bind main = fn.0
func fn.0() -> i32 {
  return i32 add(i32 call fn () -> i32 field(module bind.2(a), test)(), i32 call fn () -> i32 field(module bind.3(b), test)())
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$test()
declare i32 @$test()

define i32 @fn.0() {
  %t0 = call i32 @$test()
  %t1 = call i32 @$test()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
