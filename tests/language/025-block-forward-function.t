-- Defines a block-bodied function with an explicit return type.
add :: fn (a: i32, b: i32) -> i32 {
    return a + b
}

main :: fn () => add(20, 22)
¬
42
¬

¬
hir 0
bind add = fn.0
bind main = fn.1
func fn.0(a: i32, b: i32) -> i32 {
  return i32 add(i32 local.0(a), i32 local.1(b))
}
func fn.1() -> i32 {
  return i32 call bind.0(add)(i32 20, i32 22)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0(i32 %a, i32 %b) {
  %t0 = add i32 %a, %b
  ret i32 %t0
}

define internal i32 @fn.1() {
  %t0 = call i32 @fn.0(i32 20, i32 22)
  ret i32 %t0
}

@$add = internal alias i32 (i32, i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
