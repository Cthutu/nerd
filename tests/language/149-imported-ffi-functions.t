x :: use test.ffi_import

main :: fn () -> i32 {
    return x.absolute(-9)
}
¬
9
¬

¬
hir 0
module module.0(149-imported-ffi-functions.input)
import module.1(test.ffi_import)
import import.0 absolute from module.1(test.ffi_import).decl.0: fn (i32) -> i32
bind absolute = import.0
bind x = module.1
bind main = fn.0
func fn.0() -> i32 {
  return i32 call fn (i32) -> i32 field(module bind.1(x), absolute)(i32 negate(i32 9))
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @abs(i32)

define i32 @fn.0() {
  %t0 = sub i32 0, 9
  %t1 = call i32 @abs(i32 %t0)
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
