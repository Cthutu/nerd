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
  br i1 %t0, label %on.then.0, label %on.else.1
on.then.0:
  br label %on.end.2
on.else.1:
  br label %on.end.2
on.end.2:
  %t1 = phi i32 [10, %on.then.0], [0, %on.else.1]
  ret i32 %t1
}
@$main = alias i32 (), ptr @fn.0
