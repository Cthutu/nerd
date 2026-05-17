use std.io

score :: fn (size: u32) -> u32 {
    return on size {
        0..3 as matched on matched == 2 => matched + 20
        0..3 => 10
        10 as size on size == 10 => size * 10
        else as size => size + 100
    }
}

main :: fn () {
    prn($"1: {score(1)}")
    prn($"2: {score(2)}")
    prn($"10: {score(10)}")
    prn($"50: {score(50)}")
}
¬
0
¬
1: 10
2: 22
10: 100
50: 150

¬
hir 0
module module.0(060-on-pattern-guards.input)
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
bind score = fn.0
bind main = fn.1
func fn.0(size: u32) -> u32 {
  return u32 on u32 local.0(size) {
    range_exclusive(u32 0, u32 3) as matched when bool equal(u32 local.1(matched), u32 2) => {
      expr u32 add(u32 local.1(matched), u32 20)
    }
    range_exclusive(u32 0, u32 3) => {
      expr u32 10
    }
    value(u32 10) as size when bool equal(u32 local.2(size), u32 10) => {
      expr u32 multiply(u32 local.2(size), u32 10)
    }
    else as size => {
      expr u32 add(u32 local.3(size), u32 100)
    }
  }
}
func fn.1() -> void {
  expr void call bind.2(prn)(string interpolate(<unknown> "1: ", u32 call bind.5(score)(u32 1)))
  expr void call bind.2(prn)(string interpolate(<unknown> "2: ", u32 call bind.5(score)(u32 2)))
  expr void call bind.2(prn)(string interpolate(<unknown> "10: ", u32 call bind.5(score)(u32 10)))
  expr void call bind.2(prn)(string interpolate(<unknown> "50: ", u32 call bind.5(score)(u32 50)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"1: \00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"2: \00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"10: \00"
@.str.m0.3 = private unnamed_addr constant [5 x i8] c"50: \00"

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

define internal i32 @fn.0(i32 %size) {
  %t0 = icmp sge i32 %size, 0
  %t1 = icmp slt i32 %size, 3
  %t2 = and i1 %t0, %t1
  br i1 %t2, label %on.guard.3, label %on.next.2
on.guard.3:
  %t3 = icmp eq i32 %size, 2
  br i1 %t3, label %on.body.1, label %on.next.2
on.body.1:
  %t4 = add i32 %size, 20
  br label %on.value.4
on.value.4:
  br label %on.end.0
on.next.2:
  %t5 = icmp sge i32 %size, 0
  %t6 = icmp slt i32 %size, 3
  %t7 = and i1 %t5, %t6
  br i1 %t7, label %on.body.5, label %on.next.6
on.body.5:
  br label %on.value.7
on.value.7:
  br label %on.end.0
on.next.6:
  %t8 = icmp eq i32 %size, 10
  br i1 %t8, label %on.guard.10, label %on.next.9
on.guard.10:
  %t9 = icmp eq i32 %size, 10
  br i1 %t9, label %on.body.8, label %on.next.9
on.body.8:
  %t10 = mul i32 %size, 10
  br label %on.value.11
on.value.11:
  br label %on.end.0
on.next.9:
  br label %on.body.12
on.body.12:
  %t11 = add i32 %size, 100
  br label %on.value.14
on.value.14:
  br label %on.end.0
on.end.0:
  %t12 = phi i32 [%t4, %on.value.4], [10, %on.value.7], [%t10, %on.value.11], [%t11, %on.value.14]
  ret i32 %t12
}

define internal void @fn.1() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 3 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = call i32 @fn.0(i32 1)
  %t4 = alloca { ptr, i64 }
  call void @to_string$u32(ptr %t4, i32 %t3)
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
  %t10 = call i32 @fn.0(i32 2)
  %t11 = alloca { ptr, i64 }
  call void @to_string$u32(ptr %t11, i32 %t10)
  call void @string_builder_append_string(ptr %t11)
  %t12 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t12, i64 %t7)
  %t13 = load { ptr, i64 }, ptr %t12
  call void @$prn({ ptr, i64 } %t13)
  %t14 = call i64 @string_builder_mark()
  %t15 = alloca { ptr, i64 }
  %t16 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 4 }, ptr %t16
  call void @to_string$string(ptr %t15, ptr %t16)
  call void @string_builder_append_string(ptr %t15)
  %t17 = call i32 @fn.0(i32 10)
  %t18 = alloca { ptr, i64 }
  call void @to_string$u32(ptr %t18, i32 %t17)
  call void @string_builder_append_string(ptr %t18)
  %t19 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t19, i64 %t14)
  %t20 = load { ptr, i64 }, ptr %t19
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 4 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = call i32 @fn.0(i32 50)
  %t25 = alloca { ptr, i64 }
  call void @to_string$u32(ptr %t25, i32 %t24)
  call void @string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t21)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  ret void
}

@$score = internal alias i32 (i32), ptr @fn.0
@$main = alias void (), ptr @fn.1
