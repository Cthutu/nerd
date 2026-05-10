main :: fn() -> i32 {
    return 42
}
¬
; nerd llvm-ir 0
define i32 @$main() {
  ret i32 42
}
