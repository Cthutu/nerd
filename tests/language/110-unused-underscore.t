helper :: fn (_: i32, _: i32, _unused: i32) -> i32 {
    _ := 1
    _ := 2
    _scratch := 10
    return 7
}

value :: fn () -> i32 {
    return 5
}

main :: fn () -> i32 {
    _ := value()
    _ := value()
    _future := 3
    return helper(1, 2, 3)
}
¬
7
¬

¬
hir 0
bind helper = fn.0
bind value = fn.1
bind main = fn.2
func fn.0(_: i32, _: i32, _unused: i32) -> i32 {
  let _: i32 = untyped integer 1
  let _: i32 = untyped integer 2
  let _scratch: i32 = untyped integer 10
  return i32 7
}
func fn.1() -> i32 {
  return i32 5
}
func fn.2() -> i32 {
  let _: i32 = i32 call bind.1(value)()
  let _: i32 = i32 call bind.1(value)()
  let _future: i32 = untyped integer 3
  return i32 call bind.0(helper)(i32 1, i32 2, i32 3)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0(i32 %_.0, i32 %_.1, i32 %_unused) {
  ret i32 7
}

define internal i32 @fn.1() {
  ret i32 5
}

define internal i32 @fn.2() {
  %t0 = call i32 @fn.1()
  %t1 = call i32 @fn.1()
  %t2 = call i32 @fn.0(i32 1, i32 2, i32 3)
  ret i32 %t2
}

@$helper = internal alias i32 (i32, i32, i32), ptr @fn.0
@$value = internal alias i32 (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
