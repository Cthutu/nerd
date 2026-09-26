choose :: fn (enabled :: bool = yes) -> i32 {
    return on enabled => 7 else 9
}

main :: fn () -> i32 {
    return choose() + choose(enabled = no)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [51 x i8] c"tests/llvm/044-compile-time-specialization.input.n\00"

define internal i32 @fn.0() {
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t1 = phi i32 [7, %on.value.3], [9, %on.value.6]
  ret i32 %t1
}

define internal i32 @fn.1(i1 %enabled) {
  %t0 = icmp eq i1 %enabled, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t1 = phi i32 [7, %on.value.3], [9, %on.value.6]
  ret i32 %t1
}

define internal i32 @fn.2() {
  %t0 = call i32 @fn.0()
  %t1 = call i32 @fn.1(i1 0)
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$choose_c_93d61481 = internal alias i32 (i1), ptr @fn.0
@$choose = internal alias i32 (i1), ptr @fn.1
@$main = hidden alias i32 (), ptr @fn.2

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)