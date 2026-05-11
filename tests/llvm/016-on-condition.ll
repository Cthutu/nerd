main :: fn() -> i32 {
    value := 2
    return on {
        value == 2 => 10
        else => 0
    }
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = icmp eq i32 2, 2
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
  %t1 = phi i32 [10, %on.value.3], [0, %on.value.6]
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.0
