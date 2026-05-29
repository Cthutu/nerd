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

@.macro.file.m0 = private unnamed_addr constant [43 x i8] c"tests/llvm/002-imports-and-exports.input.n\00"

declare i32 @$local_answer()
declare i32 @$child_answer()

define internal i32 @fn.0() {
  %t0 = call i32 @mN.fn.0()
  ret i32 %t0
}

define internal i32 @fn.1() {
  %t0 = call i32 @fn.0()
  ret i32 %t0
}

@$local_answer = alias i32 (), ptr @mN.fn.0
@$child_answer = alias i32 (), ptr @mN.fn.0
@$local = alias i32 (), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
