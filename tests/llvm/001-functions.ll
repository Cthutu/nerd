main :: fn() -> i32 {
    return 42
}
¬
; nerd llvm-ir 0
define i32 @fn.0() {
  ret i32 42
}
@$main = alias i32 (), ptr @fn.0
