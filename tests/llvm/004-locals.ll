main :: fn() -> i32 {
    left: i32 = 20
    right: i32 = 22
    return left + right
}
¬
define i32 @fn.0() {
  %t0 = add i32 20, 22
  ret i32 %t0
}
@$main = alias i32 (), ptr @fn.0
