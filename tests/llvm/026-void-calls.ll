use std.io

main :: fn() {
    prn("Hello")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [34 x i8] c"tests/llvm/026-void-calls.input.n\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Hello\00"

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)