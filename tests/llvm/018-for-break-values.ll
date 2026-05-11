with_else :: fn(limit: i32) -> i32 {
    return for i := 0; i < limit; i += 1 {
        break i
    } else {
        break -1
    }
}

labelled_value :: fn() -> i32 {
    return for $loop {
        break $loop 42
    }
}

main :: fn() -> i32 {
    return with_else(0) + labelled_value()
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0(i32 %limit) {
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  br label %for.cond.0
for.cond.0:
  %t1 = load i32, ptr %local.1
  %t2 = icmp slt i32 %t1, %limit
  br i1 %t2, label %for.body.1, label %for.else.3
for.body.1:
  %t3 = load i32, ptr %local.1
  store i32 %t3, ptr %t0, align 4
  br label %for.end.4
for.update.2:
  br label %for.cond.0
for.else.3:
  %t4 = sub i32 0, 1
  store i32 %t4, ptr %t0, align 4
  br label %for.end.4
for.end.4:
  %t5 = load i32, ptr %t0, align 4
  ret i32 %t5
}

define internal i32 @fn.1() {
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  br label %for.cond.0
for.cond.0:
  br label %for.body.1
for.body.1:
  store i32 42, ptr %t0, align 4
  br label %for.end.4
for.end.4:
  %t1 = load i32, ptr %t0, align 4
  ret i32 %t1
}

define internal i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 0)
  %t1 = call i32 @fn.1()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$with_else = internal alias i32 (i32), ptr @fn.0
@$labelled_value = internal alias i32 (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
