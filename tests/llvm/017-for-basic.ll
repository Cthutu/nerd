sum_while :: fn(limit: i32) -> i32 {
    total := 0
    i := 0
    for i < limit {
        total += i
        i += 1
    }
    return total
}

sum_c_style :: fn() -> i32 {
    total := 0
    for i := 0; i < 3; i += 1 {
        total += i
    }
    return total
}

main :: fn() -> i32 {
    return sum_while(3) + sum_c_style()
}
¬
define i32 @fn.0(i32 %limit) {
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  %local.2 = alloca i32
  store i32 0, ptr %local.2
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.2
  %t1 = icmp slt i32 %t0, %limit
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = load i32, ptr %local.1
  %t3 = load i32, ptr %local.2
  %t4 = add i32 %t2, %t3
  store i32 %t4, ptr %local.1
  %t5 = load i32, ptr %local.2
  %t6 = add i32 %t5, 1
  store i32 %t6, ptr %local.2
  br label %for.cond.0
for.end.3:
  %t7 = load i32, ptr %local.1
  ret i32 %t7
}
define i32 @fn.1() {
  %local.3 = alloca i32
  store i32 0, ptr %local.3
  %local.4 = alloca i32
  store i32 0, ptr %local.4
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.4
  %t1 = icmp slt i32 %t0, 3
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = load i32, ptr %local.3
  %t3 = load i32, ptr %local.4
  %t4 = add i32 %t2, %t3
  store i32 %t4, ptr %local.3
  br label %for.update.2
for.update.2:
  %t5 = load i32, ptr %local.4
  %t6 = add i32 %t5, 1
  store i32 %t6, ptr %local.4
  br label %for.cond.0
for.end.3:
  %t7 = load i32, ptr %local.3
  ret i32 %t7
}
define i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 3)
  %t1 = call i32 @fn.1()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}
@$sum_while = alias i32 (i32), ptr @fn.0
@$sum_c_style = alias i32 (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
