answer :: 42
magic_number :: 7

main :: fn() => answer / magic_number
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = sdiv i32 42, 7
  ret i32 %t0
}

@$main = alias i32 (), ptr @fn.0
