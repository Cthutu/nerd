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
  let count: i32 = untyped integer 0
  expr void for condition {
    body {
      expr void on bool greater_equal(i32 local.0(count), i32 5) {
    value(bool yes) => {
      return <none>
    }
  }
      expr void call bind.2(prn)(string interpolate(<unknown> "Iteration ", i32 local.0(count)))
      assign i32 local.0(count) = i32 add(i32 local.0(count), i32 1)
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [11 x i8] c"Iteration \00"

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
  %t4 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = load i32, ptr %local.0
  %t6 = call { ptr, i64 } @to_string$i32(i32 %t5)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @string_builder_finish(i64 %t3)
  call void @$prn({ ptr, i64 } %t7)
  %t8 = load i32, ptr %local.0
  %t9 = add i32 %t8, 1
  store i32 %t9, ptr %local.0
  br label %for.cond.0
  ret void
}

@$main = alias void (), ptr @fn.0
