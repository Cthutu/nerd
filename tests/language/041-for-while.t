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
module module.0(041-for-while.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind main = fn.0
func fn.0() -> void {
  let i: i32 = untyped integer 0
  expr void for condition {
    condition bool less(i32 local.0(i), i32 5)
    body {
      expr void call bind.2(prn)(string interpolate(<unknown> "While ", i32 local.0(i)))
      assign i32 local.0(i) = i32 add(i32 local.0(i), i32 1)
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"While \00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal void @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.0
  %t1 = icmp slt i32 %t0, 5
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = call i64 @string_builder_mark()
  %t3 = alloca { ptr, i64 }
  %t4 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t4
  call void @to_string$string(ptr %t3, ptr %t4)
  call void @string_builder_append_string(ptr %t3)
  %t5 = load i32, ptr %local.0
  %t6 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t6, i32 %t5)
  call void @string_builder_append_string(ptr %t6)
  %t7 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t7, i64 %t2)
  %t8 = load { ptr, i64 }, ptr %t7
  call void @$prn({ ptr, i64 } %t8)
  %t9 = load i32, ptr %local.0
  %t10 = add i32 %t9, 1
  store i32 %t10, ptr %local.0
  br label %for.cond.0
for.end.3:
  ret void
}

@$main = alias void (), ptr @fn.0
