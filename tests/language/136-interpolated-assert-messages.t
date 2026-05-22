-- Checks assertion messages can be interpolated strings.
main :: fn () -> i32 {
    count := 3
    assert count == 3, $"count is {count}"
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let count: i32 = untyped integer 3
  assert bool equal(i32 local.0(count), i32 3), string interpolate(<unknown> "count is ", i32 local.0(count))
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [50 x i8] c"tests/language/136-interpolated-assert-messages.t\00"
@.str.m0.0 = private unnamed_addr constant [10 x i8] c"count is \00"
@.assert.source_path.m0.1 = private unnamed_addr constant [50 x i8] c"tests/language/136-interpolated-assert-messages.t\00"

declare i1 @string_eq(ptr, ptr)
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string(ptr)
declare void @string_builder_append_byte(i8)
declare void @string_builder_finish(ptr, i64)
declare void @to_string$string(ptr, ptr)
declare void @to_string$bool(ptr, i1)
declare void @to_string$i8(ptr, i8)
declare void @to_string$i16(ptr, i16)
declare void @to_string$i32(ptr, i32)
declare void @to_string$i64(ptr, i64)
declare void @to_string$u8(ptr, i8)
declare void @to_string$u16(ptr, i16)
declare void @to_string$u32(ptr, i32)
declare void @to_string$u64(ptr, i64)
declare void @to_string$isize(ptr, i64)
declare void @to_string$usize(ptr, i64)
declare void @to_string$f32(ptr, float)
declare void @to_string$f64(ptr, double)
declare void @nerd_assert(i1, ptr, i32, ptr)

define internal i32 @fn.0() {
  %t3 = alloca { ptr, i64 }
  %t7 = alloca { ptr, i64 }
  %t0 = icmp eq i32 3, 3
  %t1 = call i64 @string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 9 }, ptr %t3
  call void @to_string$string(ptr %t2, ptr %t3)
  call void @string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  store { ptr, i64 } %t6, ptr %t7
  call void @nerd_assert(i1 %t0, ptr @.assert.source_path.m0.1, i32 4, ptr %t7)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
