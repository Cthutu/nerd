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

define i32 @fn.0(i32 %limit) {
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.1
  %t1 = icmp slt i32 %t0, %limit
  br i1 %t1, label %for.body.1, label %for.else.2
for.body.1:
  %t2 = load i32, ptr %local.1
  br label %for.end.3
for.else.2:
  %t3 = sub i32 0, 1
  br label %for.end.3
for.end.3:
  %t4 = phi i32 [%t2, %for.body.1], [%t3, %for.else.2]
  ret i32 %t4
}

define i32 @fn.1() {
  br label %for.cond.0
for.cond.0:
  br label %for.body.1
for.body.1:
  br label %for.end.3
for.end.3:
  %t0 = phi i32 [42, %for.body.1]
  ret i32 %t0
}

define i32 @fn.2() {
  %t0 = call i32 @fn.0(i32 0)
  %t1 = call i32 @fn.1()
  %t2 = add i32 %t0, %t1
  ret i32 %t2
}

@$with_else = internal alias i32 (i32), ptr @fn.0
@$labelled_value = internal alias i32 (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2

