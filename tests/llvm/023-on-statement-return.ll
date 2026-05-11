first_positive :: fn(value: i32) -> i32 {
    on value > 0 => return value
    return 0
}

main :: fn() -> i32 {
    return first_positive(3)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0(i32 %value) {
  %t0 = icmp sgt i32 %value, 0
  %t1 = icmp eq i1 %t0, 1
  br i1 %t1, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 %value
on.end.0:
  ret i32 0
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0(i32 3)
  ret i32 %t0
}

@$first_positive = internal alias i32 (i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

