-- Checks passing assertions with and without an explicit message.
main :: fn () -> i32 {
    assert 2 < 3
    assert 4 == 4, "math still works"
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  assert bool less(untyped integer 2, untyped integer 3)
  assert bool equal(untyped integer 4, untyped integer 4), string "math still works"
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [17 x i8] c"math still works\00"
@.assert.source_path.m0.0 = private unnamed_addr constant [39 x i8] c"tests/language/107-assert-statements.t\00"
@.assert.source_path.m0.1 = private unnamed_addr constant [39 x i8] c"tests/language/107-assert-statements.t\00"
@.assert.default.m0 = private unnamed_addr constant [17 x i8] c"assertion failed\00"

declare void @nerd_assert(i1, ptr, i32, ptr)

define internal i32 @fn.0() {
  %t0 = icmp slt i32 2, 3
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.assert.default.m0, i64 16 }, ptr %t1
  call void @nerd_assert(i1 %t0, ptr @.assert.source_path.m0.0, i32 3, ptr %t1)
  %t2 = icmp eq i32 4, 4
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 16 }, ptr %t3
  call void @nerd_assert(i1 %t2, ptr @.assert.source_path.m0.1, i32 4, ptr %t3)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
