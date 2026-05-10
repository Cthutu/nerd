increment_first :: fn(values: []i32) -> i32 {
    for value in values {
        value^ += 1
        break
    }
    return values[0]
}

main :: fn() -> i32 {
    values := [1, 2]
    return increment_first(values[..])
}
¬
define i32 @fn.0({ ptr, i64 } %values) {
  %t0 = extractvalue { ptr, i64 } %values, 0
  %t1 = extractvalue { ptr, i64 } %values, 1
  %t2 = alloca i64
  store i64 0, ptr %t2
  %local.1 = alloca ptr
  br label %for.in.cond.0
for.in.cond.0:
  %t3 = load i64, ptr %t2
  %t4 = icmp ult i64 %t3, %t1
  br i1 %t4, label %for.in.body.1, label %for.in.end.2
for.in.body.1:
  %t5 = getelementptr inbounds i32, ptr %t0, i64 %t3
  store ptr %t5, ptr %local.1
  %t6 = load ptr, ptr %local.1
  %t7 = load i32, ptr %t6
  %t8 = add i32 %t7, 1
  %t9 = load ptr, ptr %local.1
  store i32 %t8, ptr %t9
  br label %for.in.end.2
for.in.end.2:
  %t10 = extractvalue { ptr, i64 } %values, 0
  %t11 = getelementptr inbounds i32, ptr %t10, i32 0
  %t12 = load i32, ptr %t11
  ret i32 %t12
}
define i32 @fn.1() {
  %local.2 = alloca [2 x i32]
  store [2 x i32] [i32 1, i32 2], ptr %local.2
  %t0 = getelementptr inbounds [2 x i32], ptr %local.2, i64 0, i64 0
  %t1 = insertvalue { ptr, i64 } poison, ptr %t0, 0
  %t2 = insertvalue { ptr, i64 } %t1, i64 2, 1
  %t3 = call i32 @fn.0({ ptr, i64 } %t2)
  ret i32 %t3
}
@$increment_first = alias i32 ({ ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
