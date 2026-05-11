main :: fn() -> i32 {
    assert 1 < 2, "ok"
    value := 1
    defer value = 2
    return value
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ok\00"
@.assert.source_path.m0.0 = private unnamed_addr constant [58 x i8] c"/home/matt/nerd/tests/llvm/014-control-statements.input.n\00"

declare void @nerd_assert(i1, ptr, i32, { ptr, i64 })

define internal i32 @fn.0() {
  %t0 = icmp slt i32 1, 2
  call void @nerd_assert(i1 %t0, ptr @.assert.source_path.m0.0, i32 2, { ptr, i64 } { ptr @.str.m0.0, i64 2 })
  %local.0 = alloca i32
  store i32 2, ptr %local.0
  ret i32 1
}

@$main = alias i32 (), ptr @fn.0
