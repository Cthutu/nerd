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

@.str.m0.0 = private unnamed_addr constant [10 x i8] c"count is \00"
@.source_path.m0 = private unnamed_addr constant [72 x i8] c"tests/language/136-interpolated-assert-messages.t\00"

declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string({ ptr, i64 })
declare void @string_builder_append_byte(i8)
declare { ptr, i64 } @string_builder_finish(i64)
declare { ptr, i64 } @to_string$string({ ptr, i64 })
declare { ptr, i64 } @to_string$bool(i1)
declare { ptr, i64 } @to_string$i8(i8)
declare { ptr, i64 } @to_string$i16(i16)
declare { ptr, i64 } @to_string$i32(i32)
declare { ptr, i64 } @to_string$i64(i64)
declare { ptr, i64 } @to_string$u8(i8)
declare { ptr, i64 } @to_string$u16(i16)
declare { ptr, i64 } @to_string$u32(i32)
declare { ptr, i64 } @to_string$u64(i64)
declare { ptr, i64 } @to_string$isize(i64)
declare { ptr, i64 } @to_string$usize(i64)
declare { ptr, i64 } @to_string$f32(float)
declare { ptr, i64 } @to_string$f64(double)
declare void @nerd_assert(i1, ptr, i32, { ptr, i64 })

define i32 @fn.0() {
  %t0 = icmp eq i32 3, 3
  %t1 = call i64 @string_builder_mark()
  %t2 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 9 })
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @to_string$i32(i32 3)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @string_builder_finish(i64 %t1)
  call void @nerd_assert(i1 %t0, ptr @.source_path.m0, i32 4, { ptr, i64 } %t4)
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
