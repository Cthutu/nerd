main :: fn() -> i32 {
    value := 2
    return on {
        value == 2 => 10
        else => 0
    }
}
¬
define i32 @fn.0() {
  %t0 = icmp eq i32 2, 2
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.end.0
on.next.2:
  br label %on.body.3
on.body.3:
  br label %on.end.0
on.end.0:
  %t1 = phi i32 [10, %on.body.1], [0, %on.body.3]
  ret i32 %t1
}
@$main = alias i32 (), ptr @fn.0
