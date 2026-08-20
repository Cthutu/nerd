main :: fn() -> i32 {
    values: []f32 = [-1.0, 2.0, -3.5]
    return values.count.as(i32)
}
¬
@.slice.const.m0.5 = private unnamed_addr constant [3 x float] [float 0xBFF0000000000000, float 0x4000000000000000, float 0xC00C000000000000]

define internal i32 @fn.0() {
  %t0 = fneg float 0x3FF0000000000000
  %t1 = fneg float 0x400C000000000000
  %t2 = insertvalue [3 x float] poison, float %t0, 0
  %t3 = insertvalue [3 x float] %t2, float 0x4000000000000000, 1
  %t4 = insertvalue [3 x float] %t3, float %t1, 2
  %t5 = alloca [3 x float]
  store [3 x float] %t4, ptr %t5
  %t6 = getelementptr inbounds [3 x float], ptr %t5, i64 0, i64 0
  %t7 = insertvalue { ptr, i64 } poison, ptr %t6, 0
  %t8 = insertvalue { ptr, i64 } %t7, i64 3, 1
  %t9 = extractvalue { ptr, i64 } %t8, 1
  %t10 = trunc i64 %t9 to i32
  ret i32 %t10
}
