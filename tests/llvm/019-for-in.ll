sum_for_in :: fn(values: []i32) -> i32 {
    total := 0
    for index, value in values {
        total += value^ + index.as(i32)
    }
    return total
}

main :: fn() -> i32 {
    values := [1, 2]
    return sum_for_in(values[..])
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [30 x i8] c"tests/llvm/019-for-in.input.n\00"

define internal i32 @fn.0({ ptr, i64 } %values) {
  %local.1 = alloca i32
  %local.2 = alloca i64
  %local.3 = alloca ptr
  store i32 0, ptr %local.1
  %t0 = extractvalue { ptr, i64 } %values, 0
  %t1 = extractvalue { ptr, i64 } %values, 1
  store i64 0, ptr %local.2
  br label %for.in.cond.0
for.in.cond.0:
  %t2 = load i64, ptr %local.2
  %t3 = icmp ult i64 %t2, %t1
  br i1 %t3, label %for.in.body.1, label %for.in.end.3
for.in.body.1:
  %t4 = getelementptr inbounds i32, ptr %t0, i64 %t2
  store ptr %t4, ptr %local.3
  %t5 = load i32, ptr %local.1
  %t6 = load ptr, ptr %local.3
  %t7 = load i32, ptr %t6
  %t8 = load i64, ptr %local.2
  %t9 = trunc i64 %t8 to i32
  %t10 = add i32 %t7, %t9
  %t11 = add i32 %t5, %t10
  store i32 %t11, ptr %local.1
  br label %for.in.update.2
for.in.update.2:
  %t12 = load i64, ptr %local.2
  %t13 = add i64 %t12, 1
  store i64 %t13, ptr %local.2
  br label %for.in.cond.0
for.in.end.3:
  %t14 = load i32, ptr %local.1
  ret i32 %t14
}

define internal i32 @fn.1() {
  %local.4 = alloca [2 x i32]
  %t0 = insertvalue [2 x i32] poison, i32 1, 0
  %t1 = insertvalue [2 x i32] %t0, i32 2, 1
  store [2 x i32] %t1, ptr %local.4
  %t2 = getelementptr inbounds [2 x i32], ptr %local.4, i64 0, i64 0
  %t3 = insertvalue { ptr, i64 } poison, ptr %t2, 0
  %t4 = insertvalue { ptr, i64 } %t3, i64 2, 1
  %t5 = call i32 @fn.0({ ptr, i64 } %t4)
  ret i32 %t5
}

@$sum_for_in = internal alias i32 ({ ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
