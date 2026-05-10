Choice :: enum {
    Empty
    Value(i32)
    Pair(i32, i32)
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
    return describe_choice(Pair(7, 4)) + describe_choice(Value(3))
}

¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0({ i64, i64 } %choice) {
  %t0 = extractvalue { i64, i64 } %choice, 0
  %t1 = icmp eq i64 %t0, 0
  br i1 %t1, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t2 = extractvalue { i64, i64 } %choice, 0
  %t3 = icmp eq i64 %t2, 1
  %t4 = extractvalue { i64, i64 } %choice, 1
  %t5 = trunc i64 %t4 to i32
  %t6 = and i1 %t3, 1
  br i1 %t6, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t7 = extractvalue { i64, i64 } %choice, 0
  %t8 = icmp eq i64 %t7, 2
  %t9 = extractvalue { i64, i64 } %choice, 1
  %t11 = alloca i64
  store i64 %t9, ptr %t11
  %t10 = load { i32, i32 }, ptr %t11
  %t12 = extractvalue { i32, i32 } %t10, 0
  %t13 = and i1 %t8, 1
  %t14 = extractvalue { i32, i32 } %t10, 1
  %t15 = icmp eq i32 %t14, 4
  %t16 = and i1 %t13, %t15
  br i1 %t16, label %on.body.7, label %on.next.8
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  br label %on.body.10
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.end.0:
  %t17 = phi i32 [0, %on.value.3], [%t5, %on.value.6], [%t12, %on.value.9], [99, %on.value.12]
  ret i32 %t17
}

define i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 7, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 4, 1
  %t3 = alloca i64
  store i64 0, ptr %t3
  store { i32, i32 } %t1, ptr %t3
  %t2 = load i64, ptr %t3
  %t4 = insertvalue { i64, i64 } poison, i64 2, 0
  %t5 = insertvalue { i64, i64 } %t4, i64 %t2, 1
  %t6 = call i32 @fn.0({ i64, i64 } %t5)
  %t7 = zext i32 3 to i64
  %t8 = insertvalue { i64, i64 } poison, i64 1, 0
  %t9 = insertvalue { i64, i64 } %t8, i64 %t7, 1
  %t10 = call i32 @fn.0({ i64, i64 } %t9)
  %t11 = add i32 %t6, %t10
  ret i32 %t11
}

@$describe_choice = alias i32 ({ i64, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
