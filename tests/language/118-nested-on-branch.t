use std.io

main :: fn () -> i32 {
    value := 2
    hit := 0

    on value == 2 => {
        on {
            value == 1 => hit = 1
            else => hit = 2
        }
    }

    prn($"hit {hit}")
    return hit
}
¬
2
¬
hit 2

¬
hir 0
module module.0(118-nested-on-branch.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> i32 {
  let value: i32 = untyped integer 2
  let hit: i32 = untyped integer 0
  expr void on bool equal(i32 local.0(value), i32 2) {
    value(bool yes) => {
      expr void on condition {
    bool equal(i32 local.0(value), i32 1) => {
      assign i32 local.1(hit) = i32 1
    }
    else => {
      assign i32 local.1(hit) = i32 2
    }
  }
    }
  }
  expr void call bind.0(prn)(string interpolate(<unknown> "hit ", i32 local.1(hit)))
  return i32 local.1(hit)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [60 x i8] c"tests/language/118-nested-on-branch.t\00"
@.str.m0.0 = private unnamed_addr constant [5 x i8] c"hit \00"

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
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  %t0 = icmp eq i32 2, 2
  %t1 = icmp eq i1 %t0, 1
  br i1 %t1, label %on.body.1, label %on.end.0
on.body.1:
  %t2 = icmp eq i32 2, 1
  br i1 %t2, label %on.body.3, label %on.next.4
on.body.3:
  store i32 1, ptr %local.1
  br label %on.end.2
on.next.4:
  br label %on.body.5
on.body.5:
  store i32 2, ptr %local.1
  br label %on.end.2
on.end.2:
  br label %on.end.0
on.end.0:
  %t3 = call i64 @string_builder_mark()
  %t4 = alloca { ptr, i64 }
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t5
  call void @to_string$string(ptr %t4, ptr %t5)
  call void @string_builder_append_string(ptr %t4)
  %t6 = load i32, ptr %local.1
  %t7 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t7, i32 %t6)
  call void @string_builder_append_string(ptr %t7)
  %t8 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t8, i64 %t3)
  %t9 = load { ptr, i64 }, ptr %t8
  call void @$prn({ ptr, i64 } %t9)
  %t10 = load i32, ptr %local.1
  ret i32 %t10
}

@$main = alias i32 (), ptr @fn.0
