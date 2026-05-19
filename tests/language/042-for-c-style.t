use std.io

main :: fn () {
    total := 0
    for i := 0, step := 1; i < 5; total += step, i += 1 {
        prn($"C {i}")
    }
    return total
}
¬
5
¬
C 0
C 1
C 2
C 3
C 4

¬
hir 0
module module.0(042-for-c-style.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  expr void for c_style {
    init {
      let i: i32 = untyped integer 0
      let step: i32 = untyped integer 1
    }
    condition bool less(i32 local.1(i), i32 5)
    body {
      expr void call bind.0(prn)(string interpolate(<unknown> "C ", i32 local.1(i)))
    }
    update {
      assign i32 local.0(total) = i32 add(i32 local.0(total), i32 local.2(step))
      assign i32 local.1(i) = i32 add(i32 local.1(i), i32 1)
    }
  }
  return i32 local.0(total)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [55 x i8] c"tests/language/042-for-c-style.t\00"
@.str.m0.0 = private unnamed_addr constant [3 x i8] c"C \00"

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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.1
  %t1 = icmp slt i32 %t0, 5
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = call i64 @string_builder_mark()
  %t3 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 2 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = load i32, ptr %local.1
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t7, i64 %t2)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn({ ptr, i64 } %t8)
  br label %for.update.2
for.update.2:
  %t9 = load i32, ptr %local.0
  %t10 = add i32 %t9, 1
  store i32 %t10, ptr %local.0
  %t11 = load i32, ptr %local.1
  %t12 = add i32 %t11, 1
  store i32 %t12, ptr %local.1
  br label %for.cond.0
for.end.3:
  %t13 = load i32, ptr %local.0
  ret i32 %t13
}

@$main = alias i32 (), ptr @fn.0
