main :: fn() -> i32 {
    values: []f32 = [-1.0, 2.0, -3.5]
    return values.count.as(i32)
}
¬
@.slice.const.m0.5 = private unnamed_addr constant [3 x float] [float 0xBFF0000000000000, float 0x4000000000000000, float 0xC00C000000000000]

define internal i32 @fn.0() {
  %t0 = extractvalue { ptr, i64 } { ptr @.slice.const.m0.5, i64 3 }, 1
  %t1 = trunc i64 %t0 to i32
  ret i32 %t1
}
