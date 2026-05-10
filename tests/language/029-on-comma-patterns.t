use std.io

-- Matches multiple integer values in one block-form `on` branch.
size :: 2

test_branch :: fn (size: u32) -> i32 {
    return on size {
        0, 1 => 10
        else => 30
    }
}

main :: fn () {
    prn($"0: {test_branch(0)}")
    prn($"1: {test_branch(1)}")
    prn($"2: {test_branch(2)}")

    return test_branch(size)
}
¬
30
¬
0: 10
1: 10
2: 30

¬
hir 0
module module.0(029-on-comma-patterns.input)
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
bind size = value.0
bind test_branch = fn.0
bind main = fn.1
const value.0: untyped integer = untyped integer 2
func fn.0(size: u32) -> i32 {
  return i32 on u32 local.0(size) {
    value(u32 0), value(u32 1) => {
      expr i32 10
    }
    else => {
      expr i32 30
    }
  }
}
func fn.1() -> i32 {
  expr void call bind.2(prn)(string interpolate(<unknown> "0: ", i32 call bind.6(test_branch)(u32 0)))
  expr void call bind.2(prn)(string interpolate(<unknown> "1: ", i32 call bind.6(test_branch)(u32 1)))
  expr void call bind.2(prn)(string interpolate(<unknown> "2: ", i32 call bind.6(test_branch)(u32 2)))
  return i32 call bind.6(test_branch)(u32 bind.5(size))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"0: \00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"1: \00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"2: \00"

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

define i32 @fn.0(i32 %size) {
  %t0 = icmp eq i32 %size, 0
  %t1 = icmp eq i32 %size, 1
  %t2 = or i1 %t0, %t1
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t3 = phi i32 [10, %on.value.3], [30, %on.value.6]
  ret i32 %t3
}

define i32 @fn.1() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = call i32 @fn.0(i32 0)
  %t3 = call { ptr, i64 } @to_string$i32(i32 %t2)
  call void @string_builder_append_string({ ptr, i64 } %t3)
  %t4 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t4)
  %t5 = call i64 @string_builder_mark()
  %t6 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 3 })
  call void @string_builder_append_string({ ptr, i64 } %t6)
  %t7 = call i32 @fn.0(i32 1)
  %t8 = call { ptr, i64 } @to_string$i32(i32 %t7)
  call void @string_builder_append_string({ ptr, i64 } %t8)
  %t9 = call { ptr, i64 } @string_builder_finish(i64 %t5)
  call void @$prn({ ptr, i64 } %t9)
  %t10 = call i64 @string_builder_mark()
  %t11 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 3 })
  call void @string_builder_append_string({ ptr, i64 } %t11)
  %t12 = call i32 @fn.0(i32 2)
  %t13 = call { ptr, i64 } @to_string$i32(i32 %t12)
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = call { ptr, i64 } @string_builder_finish(i64 %t10)
  call void @$prn({ ptr, i64 } %t14)
  %t15 = call i32 @fn.0(i32 2)
  ret i32 %t15
}

@$test_branch = alias i32 (i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
