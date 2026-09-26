use std.io

main :: fn () {
    i := 0
    for i < 5 {
        prn($"While {i}")
        i += 1
    }
}
¬
0
¬
While 0
While 1
While 2
While 3
While 4

¬
hir 0
module module.041-for-while.input(041-for-while.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> void {
  let i: i32 = untyped integer 0
  expr void for condition {
    condition bool less(i32 local.0(i), i32 5)
    body {
      expr void call bind.2(prn_text)(string interpolate(<unknown> "While ", i32 local.0(i)))
      assign i32 local.0(i) = i32 add(i32 local.0(i), i32 1)
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [31 x i8] c"tests/language/041-for-while.t\00"
@.str.m0.0 = private unnamed_addr constant [7 x i8] c"While \00"

declare i1 @nrt_string_eq(ptr, ptr)
declare void @nrt_string_builder_reset()
declare i64 @nrt_string_builder_mark()
declare void @nrt_string_builder_append_string(ptr)
declare void @nrt_string_builder_append_byte(i8)
declare void @nrt_string_builder_finish(ptr, i64)
declare void @nrt_string_builder_finish_in(ptr, i64, ptr)
declare void @nrt_to_string_string(ptr, ptr)
declare void @nrt_to_string_bool(ptr, i1)
declare void @nrt_to_string_i8(ptr, i8)
declare void @nrt_to_string_i16(ptr, i16)
declare void @nrt_to_string_i32(ptr, i32)
declare void @nrt_to_string_i64(ptr, i64)
declare void @nrt_to_string_u8(ptr, i8)
declare void @nrt_to_string_u16(ptr, i16)
declare void @nrt_to_string_u32(ptr, i32)
declare void @nrt_to_string_u64(ptr, i64)
declare void @nrt_to_string_isize(ptr, i64)
declare void @nrt_to_string_usize(ptr, i64)
declare void @nrt_to_string_f32(ptr, float)
declare void @nrt_to_string_f64(ptr, double)

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  %local.0 = alloca i32
  %t4 = alloca { ptr, i64 }
  store i32 0, ptr %local.0
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.0
  %t1 = icmp slt i32 %t0, 5
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = call i64 @nrt_string_builder_mark()
  %t3 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t4
  call void @nrt_to_string_string(ptr %t3, ptr %t4)
  call void @nrt_string_builder_append_string(ptr %t3)
  %t5 = load i32, ptr %local.0
  %t6 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t6, i32 %t5)
  call void @nrt_string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t7, i64 %t2)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn_text({ ptr, i64 } %t8)
  %t9 = load i32, ptr %local.0
  %t10 = add i32 %t9, 1
  store i32 %t10, ptr %local.0
  br label %for.cond.0
for.end.3:
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)