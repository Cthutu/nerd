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
define i32 @fn.0(i32 %value) {
  %t0 = icmp eq i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.end.0
on.next.2:
  %t1 = icmp sge i32 %value, 1
  %t2 = icmp slt i32 %value, 3
  %t3 = and i1 %t1, %t2
  br i1 %t3, label %on.guard.5, label %on.next.4
on.guard.5:
  %t4 = icmp eq i32 %value, 2
  br i1 %t4, label %on.body.3, label %on.next.4
on.body.3:
  %t5 = add i32 %value, 20
  br label %on.end.0
on.next.4:
  br label %on.body.6
on.body.6:
  %t6 = add i32 %value, 100
  br label %on.end.0
on.end.0:
  %t7 = phi i32 [10, %on.body.1], [%t5, %on.body.3], [%t6, %on.body.6]
  ret i32 %t7
}
define i32 @fn.1(i32 %value) {
  %t0 = icmp eq i32 %value, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.end.0
on.next.2:
  %t1 = icmp ne i32 %value, 1
  br i1 %t1, label %on.body.3, label %on.next.4
on.body.3:
  br label %on.end.0
on.next.4:
  %t2 = icmp slt i32 %value, 4
  br i1 %t2, label %on.body.5, label %on.next.6
on.body.5:
  br label %on.end.0
on.next.6:
  %t3 = icmp sle i32 %value, 5
  br i1 %t3, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.end.0
on.next.8:
  %t4 = icmp sgt i32 %value, 6
  br i1 %t4, label %on.body.9, label %on.next.10
on.body.9:
  br label %on.end.0
on.next.10:
  %t5 = icmp sge i32 %value, 7
  br i1 %t5, label %on.body.11, label %on.next.12
on.body.11:
  br label %on.end.0
on.next.12:
  br label %on.body.13
on.body.13:
  br label %on.end.0
on.end.0:
  %t6 = phi i32 [1, %on.body.1], [2, %on.body.3], [3, %on.body.5], [4, %on.body.7], [5, %on.body.9], [6, %on.body.11], [7, %on.body.13]
  ret i32 %t6
}
define i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 2)
  %t1 = call i32 @fn.1(i32 8)
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}
@$classify = alias i32 (i32), ptr @fn.0
@$compare_pattern = alias i32 (i32), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
