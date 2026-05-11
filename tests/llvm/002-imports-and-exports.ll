pub use test.folder_pub_use

pub local :: fn() -> i32 {
    return child_answer()
}

main :: fn() -> i32 {
    return local()
}
¬
; nerd llvm-ir 0
; generated from HIR

declare i32 @$local_answer()
declare i32 @$child_answer()

define i32 @fn.0() {
  %t0 = call i32 @$child_answer()
  ret i32 %t0
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0()
  ret i32 %t0
}

@$local = alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

