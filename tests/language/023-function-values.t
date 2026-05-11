-- Stores an anonymous function value in a binding and calls it.
adder := fn (a: i32, b: i32) => a + b
main :: fn () => adder(20, 22)
¬
42
¬

¬
hir 0
bind adder = value.0
bind main = fn.1
global value.0: fn (i32, i32) -> i32 = fn (i32, i32) -> i32 fn.0
func fn.0(a: i32, b: i32) -> i32 {
  return i32 add(i32 local.0(a), i32 local.1(b))
}
func fn.1() -> i32 {
  return i32 call bind.0(adder)(i32 20, i32 22)
}
¬
; nerd llvm-ir 0
; generated from HIR

@$adder = internal global ptr null

define void @m0.init() {
  store ptr @fn.0, ptr @$adder
  ret void
}

define internal i32 @fn.0(i32 %a, i32 %b) {
  %t0 = add i32 %a, %b
  ret i32 %t0
}

define internal i32 @fn.1() {
  %t0 = load ptr, ptr @$adder
  %t1 = call i32 %t0(i32 20, i32 22)
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.1
