Choice :: enum {
    Empty
    Value(i32)
    Pair(i32, i32)
}

make_pair :: fn() -> Choice {
    return Pair(7, 4)
}

make_value :: fn() -> Choice {
    return Value(3)
}

describe_choice :: fn(choice: Choice) -> i32 {
    return on choice {
        Empty() => 0
        Value(as value) => value
        Pair(as left, 4) => left
        else => 99
    }
}

main :: fn() -> i32 {
    return describe_choice(make_pair()) + describe_choice(make_value())
}
¬
define { i64, { i32, i32 } } @fn.0() {
  %t0 = insertvalue { i32, i32 } poison, i32 7, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 4, 1
  %t2 = insertvalue { i64, { i32, i32 } } poison, i64 2, 0
  %t3 = insertvalue { i64, { i32, i32 } } %t2, { i32, i32 } %t1, 1
  ret { i64, { i32, i32 } } %t3
}
define { i64, { i32, i32 } } @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 3, 0
  %t1 = insertvalue { i64, { i32, i32 } } poison, i64 1, 0
  %t2 = insertvalue { i64, { i32, i32 } } %t1, { i32, i32 } %t0, 1
  ret { i64, { i32, i32 } } %t2
}
define i32 @fn.2({ i64, { i32, i32 } } %choice) {
  %t0 = extractvalue { i64, { i32, i32 } } %choice, 0
  %t1 = icmp eq i64 %t0, 0
  br i1 %t1, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.end.0
on.next.2:
  %t2 = extractvalue { i64, { i32, i32 } } %choice, 0
  %t3 = icmp eq i64 %t2, 1
  %t4 = extractvalue { i64, { i32, i32 } } %choice, 1
  %t5 = extractvalue { i32, i32 } %t4, 0
  %t6 = and i1 %t3, 1
  br i1 %t6, label %on.body.3, label %on.next.4
on.body.3:
  br label %on.end.0
on.next.4:
  %t7 = extractvalue { i64, { i32, i32 } } %choice, 0
  %t8 = icmp eq i64 %t7, 2
  %t9 = extractvalue { i64, { i32, i32 } } %choice, 1
  %t10 = extractvalue { i32, i32 } %t9, 0
  %t11 = and i1 %t8, 1
  %t12 = extractvalue { i32, i32 } %t9, 1
  %t13 = icmp eq i32 %t12, 4
  %t14 = and i1 %t11, %t13
  br i1 %t14, label %on.body.5, label %on.next.6
on.body.5:
  br label %on.end.0
on.next.6:
  br label %on.body.7
on.body.7:
  br label %on.end.0
on.end.0:
  %t15 = phi i32 [0, %on.body.1], [%t5, %on.body.3], [%t10, %on.body.5], [99, %on.body.7]
  ret i32 %t15
}
define i32 @fn.3() {
  %t0 = call { i64, { i32, i32 } } @fn.0()
  %t1 = call i32 @fn.2({ i64, { i32, i32 } } %t0)
  %t2 = call { i64, { i32, i32 } } @fn.1()
  %t3 = call i32 @fn.2({ i64, { i32, i32 } } %t2)
  %t4 = add i32 %t1, %t3
  ret i32 %t4
}
@$make_pair = alias { i64, { i32, i32 } } (), ptr @fn.0
@$make_value = alias { i64, { i32, i32 } } (), ptr @fn.1
@$describe_choice = alias i32 ({ i64, { i32, i32 } }), ptr @fn.2
@$main = alias i32 (), ptr @fn.3
