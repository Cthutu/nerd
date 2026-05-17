use std.io

add :: fn (a: i32, b: i32 = 20, c: i32 = a + b) => a + b + c
scale :: fn (value: i32) => value * 10
with_call_default :: fn (value: i32 = scale(3)) => value + 1

local_call :: fn (a: i32) {
    inner :: fn (x: i32, y: i32 = x + 3) => x * y
    prn($"inner {inner(a)}")
}

main :: fn () {
    prn($"one {add(2)}")
    prn($"two {add(2, 3)}")
    prn($"three {add(2, 3, 4)}")
    prn($"call {with_call_default()}")
    local_call(4)
}
¬
0
¬
one 44
two 27
three 9
call 31
inner 28

¬
hir 0
module module.0(126-default-parameters.input)
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
bind add = fn.0
bind scale = fn.1
bind with_call_default = fn.2
bind local_call = fn.3
bind main = fn.5
func fn.0(a: i32, b: i32, c: i32) -> i32 {
  return i32 add(i32 add(i32 local.0(a), i32 local.1(b)), i32 local.2(c))
}
func fn.1(value: i32) -> i32 {
  return i32 multiply(i32 local.3(value), i32 10)
}
func fn.2(value: i32) -> i32 {
  return i32 add(i32 local.4(value), i32 1)
}
func fn.3(a: i32) -> void {
  let inner: fn (i32, i32) -> i32 = fn (i32, i32) -> i32 fn.4
  expr void call bind.2(prn)(string interpolate(<unknown> "inner ", i32 call local.6(inner)(i32 local.5(a))))
}
func fn.4(x: i32, y: i32) -> i32 {
  return i32 multiply(i32 local.7(x), i32 local.8(y))
}
func fn.5() -> void {
  expr void call bind.2(prn)(string interpolate(<unknown> "one ", i32 call bind.5(add)(i32 2)))
  expr void call bind.2(prn)(string interpolate(<unknown> "two ", i32 call bind.5(add)(i32 2, i32 3)))
  expr void call bind.2(prn)(string interpolate(<unknown> "three ", i32 call bind.5(add)(i32 2, i32 3, i32 4)))
  expr void call bind.2(prn)(string interpolate(<unknown> "call ", i32 call bind.7(with_call_default)()))
  expr void call bind.8(local_call)(i32 4)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [7 x i8] c"inner \00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"one \00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"two \00"
@.str.m0.3 = private unnamed_addr constant [7 x i8] c"three \00"
@.str.m0.4 = private unnamed_addr constant [6 x i8] c"call \00"

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

define internal i32 @fn.0(i32 %a, i32 %b, i32 %c) {
  %t0 = add i32 %a, %b
  %t1 = add i32 %t0, %c
  ret i32 %t1
}

define internal i32 @fn.1(i32 %value) {
  %t0 = mul i32 %value, 10
  ret i32 %t0
}

define internal i32 @fn.2(i32 %value) {
  %t0 = add i32 %value, 1
  ret i32 %t0
}

define internal void @fn.3(i32 %a) {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 6 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = add i32 %a, 3
  %t4 = call i32 @fn.4(i32 %a, i32 %t3)
  %t5 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t5, i32 %t4)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t6, i64 %t0)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn({ ptr, i64 } %t7)
  ret void
}

define internal i32 @fn.4(i32 %x, i32 %y) {
  %t0 = mul i32 %x, %y
  ret i32 %t0
}

define internal void @fn.5() {
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 4 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = add i32 2, 20
  %t4 = call i32 @fn.0(i32 2, i32 20, i32 %t3)
  %t5 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t5, i32 %t4)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t6, i64 %t0)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn({ ptr, i64 } %t7)
  %t8 = call i64 @string_builder_mark()
  %t9 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 4 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = add i32 2, 20
  %t12 = call i32 @fn.0(i32 2, i32 3, i32 %t11)
  %t13 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t13, i32 %t12)
  call void @string_builder_append_string(ptr %t13)
  %t14 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t14, i64 %t8)
  %t15 = load { ptr, i64 }, ptr %t14
  call void @$prn({ ptr, i64 } %t15)
  %t16 = call i64 @string_builder_mark()
  %t17 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 6 }, ptr %t18
  call void @to_string$string(ptr %t17, ptr %t18)
  call void @string_builder_append_string(ptr %t17)
  %t19 = add i32 2, 20
  %t20 = call i32 @fn.0(i32 2, i32 3, i32 4)
  %t21 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t21, i32 %t20)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t22, i64 %t16)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn({ ptr, i64 } %t23)
  %t24 = call i64 @string_builder_mark()
  %t25 = alloca { ptr, i64 }
  %t26 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 5 }, ptr %t26
  call void @to_string$string(ptr %t25, ptr %t26)
  call void @string_builder_append_string(ptr %t25)
  %t27 = call i32 @fn.1(i32 3)
  %t28 = call i32 @fn.2(i32 %t27)
  %t29 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t29, i32 %t28)
  call void @string_builder_append_string(ptr %t29)
  %t30 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t30, i64 %t24)
  %t31 = load { ptr, i64 }, ptr %t30
  call void @$prn({ ptr, i64 } %t31)
  call void @fn.3(i32 4)
  ret void
}

@$add = internal alias i32 (i32, i32, i32), ptr @fn.0
@$scale = internal alias i32 (i32), ptr @fn.1
@$with_call_default = internal alias i32 (i32), ptr @fn.2
@$local_call = internal alias void (i32), ptr @fn.3
@$main = alias void (), ptr @fn.5
