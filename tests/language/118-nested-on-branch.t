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
func fn.0() -> i32 {
  let value: i32 = untyped integer 2
  let hit: i32 = untyped integer 0
  expr void on bool equal(i32 local.0(value), i32 2) {
    value(bool yes) => {
      expr void block {
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
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "hit ", i32 local.1(hit)))
  return i32 local.1(hit)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"hit \00"

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

define i32 @fn.0() {
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  %t0 = icmp eq i32 2, 2
  %t1 = icmp eq i1 %t0, 1
  br i1 %t1, label %on.body.1, label %on.end.0
on.body.1:
  %t2 = icmp eq i32 2, 1
  br i1 %t2, label %on.body.4, label %on.next.5
on.body.4:
  store i32 1, ptr %local.1
  br label %on.end.3
on.next.5:
  br label %on.body.6
on.body.6:
  store i32 2, ptr %local.1
  br label %on.end.3
on.end.3:
  br label %block.end.2
block.end.2:
  br label %on.end.0
on.end.0:
  %t3 = call i64 @string_builder_mark()
  %t4 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = load i32, ptr %local.1
  %t6 = call { ptr, i64 } @to_string$i32(i32 %t5)
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call { ptr, i64 } @string_builder_finish(i64 %t3)
  call void @$prn({ ptr, i64 } %t7)
  %t8 = load i32, ptr %local.1
  ret i32 %t8
}

@$main = alias i32 (), ptr @fn.0
