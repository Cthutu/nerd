-- Resolves a local function binding through a forward reference.
main :: fn () {
    result := add(20, 22)
    add :: fn (a: i32, b: i32) => a + b
    return result
}
¬
42
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let result: i32 = i32 call local.0(add)(i32 20, i32 22)
  let add: fn (i32, i32) -> i32 = fn (i32, i32) -> i32 fn.1
  return i32 local.1(result)
}
func fn.1(a: i32, b: i32) -> i32 {
  return i32 add(i32 local.2(a), i32 local.3(b))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = call i32 @fn.1(i32 20, i32 22)
  ret i32 %t0
}

define internal i32 @fn.1(i32 %a, i32 %b) {
  %t0 = add i32 %a, %b
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
