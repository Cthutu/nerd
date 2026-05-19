use std.io

-- Matches exclusive and inclusive integer ranges in block-form `on` branches.
test_branch :: fn (size: u32) -> i32 {
    return on size {
        0..2 => 10
        2..=4 => 20
        else => 30
    }
}

main :: fn () {
    prn($"0: {test_branch(0)}")
    prn($"1: {test_branch(1)}")
    prn($"2: {test_branch(2)}")
    prn($"4: {test_branch(4)}")
    prn($"5: {test_branch(5)}")

    return test_branch(5)
}
¬
30
¬
0: 10
1: 10
2: 20
4: 20
5: 30

¬
hir 0
module module.0(030-on-ranges.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind test_branch = fn.0
bind main = fn.1
func fn.0(size: u32) -> i32 {
  return i32 on u32 local.0(size) {
    range_exclusive(u32 0, u32 2) => {
      expr i32 10
    }
    range_inclusive(u32 2, u32 4) => {
      expr i32 20
    }
    else => {
      expr i32 30
    }
  }
}
func fn.1() -> i32 {
  expr void call bind.0(prn)(string interpolate(<unknown> "0: ", i32 call bind.2(test_branch)(u32 0)))
  expr void call bind.0(prn)(string interpolate(<unknown> "1: ", i32 call bind.2(test_branch)(u32 1)))
  expr void call bind.0(prn)(string interpolate(<unknown> "2: ", i32 call bind.2(test_branch)(u32 2)))
  expr void call bind.0(prn)(string interpolate(<unknown> "4: ", i32 call bind.2(test_branch)(u32 4)))
  expr void call bind.0(prn)(string interpolate(<unknown> "5: ", i32 call bind.2(test_branch)(u32 5)))
  return i32 call bind.2(test_branch)(u32 5)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [53 x i8] c"tests/language/030-on-ranges.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"0: \00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"1: \00"
@.str.m0.2 = private unnamed_addr constant [4 x i8] c"2: \00"
@.str.m0.3 = private unnamed_addr constant [4 x i8] c"4: \00"
@.str.m0.4 = private unnamed_addr constant [4 x i8] c"5: \00"

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

define internal i32 @fn.0(i32 %size) {
  %t0 = icmp sge i32 %size, 0
  %t1 = icmp slt i32 %size, 2
  %t2 = and i1 %t0, %t1
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t3 = icmp sge i32 %size, 2
  %t4 = icmp sle i32 %size, 4
  %t5 = and i1 %t3, %t4
  br i1 %t5, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  br label %on.body.7
on.body.7:
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.end.0:
  %t6 = phi i32 [10, %on.value.3], [20, %on.value.6], [30, %on.value.9]
  ret i32 %t6
}

define internal i32 @fn.1() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 3 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = call i32 @fn.0(i32 0)
  %t4 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t4, i32 %t3)
  call void @string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t5, i64 %t0)
  %t6 = load { ptr, i64 }, ptr %t5
  call void @$prn({ ptr, i64 } %t6)
  %t7 = call i64 @string_builder_mark()
  %t8 = alloca { ptr, i64 }
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 3 }, ptr %t9
  call void @to_string$string(ptr %t8, ptr %t9)
  call void @string_builder_append_string(ptr %t8)
  %t10 = call i32 @fn.0(i32 1)
  %t11 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t11, i32 %t10)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t7)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  %t14 = call i64 @string_builder_mark()
  %t15 = alloca { ptr, i64 }
  %t16 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 3 }, ptr %t16
  call void @to_string$string(ptr %t15, ptr %t16)
  call void @string_builder_append_string(ptr %t15)
  %t17 = call i32 @fn.0(i32 2)
  %t18 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t18, i32 %t17)
  call void @string_builder_append_string(ptr %t18)
  %t19 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t19, i64 %t14)
  %t20 = load { ptr, i64 }, ptr %t19
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 3 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = call i32 @fn.0(i32 4)
  %t25 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t25, i32 %t24)
  call void @string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t21)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  %t28 = call i64 @string_builder_mark()
  %t29 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 3 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = call i32 @fn.0(i32 5)
  %t32 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t32, i32 %t31)
  call void @string_builder_append_string(ptr %t32)
  %t33 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t33, i64 %t28)
  %t34 = load { ptr, i64 }, ptr %t33
  call void @$prn({ ptr, i64 } %t34)
  %t35 = call i32 @fn.0(i32 5)
  ret i32 %t35
}

@$test_branch = internal alias i32 (i32), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
