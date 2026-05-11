classify :: fn(value: i32) -> i32 {
    return on value {
        0 => 10
        1..3 as matched on matched == 2 => matched + 20
        else as other => other + 100
    }
}

compare_pattern :: fn(value: i32) -> i32 {
    return on value {
        == 0 => 1
        != 1 => 2
        < 4 => 3
        <= 5 => 4
        > 6 => 5
        >= 7 => 6
        else => 7
    }
}

main :: fn() -> i32 {
    return classify(2) + compare_pattern(8)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0(i32 %value) {
  %t0 = icmp eq i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t1 = icmp sge i32 %value, 1
  %t2 = icmp slt i32 %value, 3
  %t3 = and i1 %t1, %t2
  br i1 %t3, label %on.guard.6, label %on.next.5
on.guard.6:
  %t4 = icmp eq i32 %value, 2
  br i1 %t4, label %on.body.4, label %on.next.5
on.body.4:
  %t5 = add i32 %value, 20
  br label %on.value.7
on.value.7:
  br label %on.end.0
on.next.5:
  br label %on.body.8
on.body.8:
  %t6 = add i32 %value, 100
  br label %on.value.10
on.value.10:
  br label %on.end.0
on.end.0:
  %t7 = phi i32 [10, %on.value.3], [%t5, %on.value.7], [%t6, %on.value.10]
  ret i32 %t7
}

define i32 @fn.1(i32 %value) {
  %t0 = icmp eq i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t1 = icmp ne i32 %value, 1
  br i1 %t1, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t2 = icmp slt i32 %value, 4
  br i1 %t2, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t3 = icmp sle i32 %value, 5
  br i1 %t3, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  %t4 = icmp sgt i32 %value, 6
  br i1 %t4, label %on.body.13, label %on.next.14
on.body.13:
  br label %on.value.15
on.value.15:
  br label %on.end.0
on.next.14:
  %t5 = icmp sge i32 %value, 7
  br i1 %t5, label %on.body.16, label %on.next.17
on.body.16:
  br label %on.value.18
on.value.18:
  br label %on.end.0
on.next.17:
  br label %on.body.19
on.body.19:
  br label %on.value.21
on.value.21:
  br label %on.end.0
on.end.0:
  %t6 = phi i32 [1, %on.value.3], [2, %on.value.6], [3, %on.value.9], [4, %on.value.12], [5, %on.value.15], [6, %on.value.18], [7, %on.value.21]
  ret i32 %t6
}

define i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 2)
  %t1 = call i32 @fn.1(i32 8)
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$classify = internal alias i32 (i32), ptr @fn.0
@$compare_pattern = internal alias i32 (i32), ptr @fn.1
@$main = alias i32 (), ptr @fn.2

