consume :: fn (values: []i32) { assert values[0] == 42 }
work :: fn (n: i32) -> i32 {
    values: [n]i32
    values[0] = 42
    defer consume(values)
    return values[0]
}
main :: fn () -> i32 { return work(3) - 42 }
¬
define internal i32 @fn.1(i32 %n) {
  %t0 = call ptr @llvm.stacksave.p0()
  %t1 = sext i32 %n to i64
  %t5 = call i64 @nrt_local_array_bytes(i64 %t1, i64 4)
  %t2 = alloca i8, i64 %t5, align 16
  call void @llvm.memset.p0.i64(ptr %t2, i8 0, i64 %t5, i1 false)
  %t3 = insertvalue { ptr, i64 } zeroinitializer, ptr %t2, 0
  %t4 = insertvalue { ptr, i64 } %t3, i64 %t1, 1
  %t6 = extractvalue { ptr, i64 } %t4, 0
  %t7 = getelementptr inbounds i32, ptr %t6, i32 0
  store i32 42, ptr %t7
  %t8 = extractvalue { ptr, i64 } %t4, 0
  %t9 = getelementptr inbounds i32, ptr %t8, i32 0
  %t10 = load i32, ptr %t9
  call void @fn.0({ ptr, i64 } %t4)
  call void @llvm.stackrestore.p0(ptr %t0)
  ret i32 %t10
}
