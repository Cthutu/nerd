on linux {
    use test.platform_ffi
}

on windows {
    use test.platform_ffi
}

main :: fn () -> i32 {
    return abs(-7) + PLATFORM_VALUE
}
¬
19
¬

¬
hir 0
module module.0(134-platform-on-use.input)
import module.1(test.platform_ffi)
import import.0 abs from module.1(test.platform_ffi).decl.0: fn (i32) -> i32
import import.1 PLATFORM_VALUE from module.1(test.platform_ffi).decl.1: untyped integer
bind abs = import.0
bind PLATFORM_VALUE = import.1
bind PLATFORM_VALUE = value.0
bind main = fn.0
bind abs = fn.1
const value.0: untyped integer
func fn.0() -> i32 {
  return i32 add(i32 call bind.0(abs)(i32 negate(i32 7)), i32 bind.2(PLATFORM_VALUE))
}
extern func fn.1(i32) -> i32
¬
; nerd llvm-ir 0
; generated from HIR
define i32 @fn.0() {
  %t0 = sub i32 0, 7
  %t1 = call i32 @abs(i32 %t0)
  %t2 = add i32 %t1, 12
  ret i32 %t2
}

declare i32 @abs(i32)

@$main = alias i32 (), ptr @fn.0
