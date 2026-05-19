use std.io

main :: fn () {
    count := 0

    for {
        on count >= 5 => return

        prn($"Iteration {count}")
        count += 1
    }
}
¬
0
¬
Iteration 0
Iteration 1
Iteration 2
Iteration 3
Iteration 4

¬
hir 0
module module.0(039-5-prints.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  let count: i32 = untyped integer 0
  expr void for condition {
    body {
      expr void on bool greater_equal(i32 local.0(count), i32 5) {
    value(bool yes) => {
      return <none>
    }
  }
      expr void call bind.0(prn)(string interpolate(<unknown> "Iteration ", i32 local.0(count)))
      assign i32 local.0(count) = i32 add(i32 local.0(count), i32 1)
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [52 x i8] c"tests/language/039-5-prints.t\00"
@.str.m0.0 = private unnamed_addr constant [11 x i8] c"Iteration \00"

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

define internal void @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  br label %for.cond.0
for.cond.0:
  br label %for.body.1
for.body.1:
  %t0 = load i32, ptr %local.0
  %t1 = icmp sge i32 %t0, 5
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.5, label %on.end.4
on.body.5:
  ret void
on.end.4:
  %t3 = call i64 @string_builder_mark()
  %t4 = alloca { ptr, i64 }
  %t5 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 10 }, ptr %t5
  call void @to_string$string(ptr %t4, ptr %t5)
  call void @string_builder_append_string(ptr %t4)
  %t6 = load i32, ptr %local.0
  %t7 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t7, i32 %t6)
  call void @string_builder_append_string(ptr %t7)
  %t8 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t8, i64 %t3)
  %t9 = load { ptr, i64 }, ptr %t8
  call void @$prn({ ptr, i64 } %t9)
  %t10 = load i32, ptr %local.0
  %t11 = add i32 %t10, 1
  store i32 %t11, ptr %local.0
  br label %for.cond.0
  ret void
}

@$main = alias void (), ptr @fn.0
