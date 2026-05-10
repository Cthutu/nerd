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

define i32 @fn.0(i32 %a, i32 %b, i32 %c) {
  %t0 = add i32 %a, %b
  %t1 = add i32 %t0, %c
  ret i32 %t1
}

define i32 @fn.1(i32 %value) {
  %t0 = mul i32 %value, 10
  ret i32 %t0
}

define i32 @fn.2(i32 %value) {
  %t0 = add i32 %value, 1
  ret i32 %t0
}

define void @fn.3(i32 %a) {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = add i32 %a, 3
  %t3 = call i32 @fn.4(i32 %a, i32 %t2)
  %t4 = call { ptr, i64 } @to_string$i32(i32 %t3)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t5)
  ret void
}

define i32 @fn.4(i32 %x, i32 %y) {
  %t0 = mul i32 %x, %y
  ret i32 %t0
}

define void @fn.5() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = add i32 2, 20
  %t3 = call i32 @fn.0(i32 2, i32 20, i32 %t2)
  %t4 = call { ptr, i64 } @to_string$i32(i32 %t3)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t5)
  %t6 = call i64 @string_builder_mark()
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = add i32 2, 20
  %t9 = call i32 @fn.0(i32 2, i32 3, i32 %t8)
  %t10 = call { ptr, i64 } @to_string$i32(i32 %t9)
  call void @string_builder_append_string({ ptr, i64 } %t10)
  %t11 = call { ptr, i64 } @string_builder_finish(i64 %t6)
  call void @$prn({ ptr, i64 } %t11)
  %t12 = call i64 @string_builder_mark()
  %t13 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t13)
  %t14 = add i32 2, 20
  %t15 = call i32 @fn.0(i32 2, i32 3, i32 4)
  %t16 = call { ptr, i64 } @to_string$i32(i32 %t15)
  call void @string_builder_append_string({ ptr, i64 } %t16)
  %t17 = call { ptr, i64 } @string_builder_finish(i64 %t12)
  call void @$prn({ ptr, i64 } %t17)
  %t18 = call i64 @string_builder_mark()
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 5 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call i32 @fn.1(i32 3)
  %t21 = call i32 @fn.2(i32 %t20)
  %t22 = call { ptr, i64 } @to_string$i32(i32 %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @string_builder_finish(i64 %t18)
  call void @$prn({ ptr, i64 } %t23)
  call void @fn.3(i32 4)
  ret void
}

@$add = alias i32 (i32, i32, i32), ptr @fn.0
@$scale = alias i32 (i32), ptr @fn.1
@$with_call_default = alias i32 (i32), ptr @fn.2
@$local_call = alias void (i32), ptr @fn.3
@$main = alias void (), ptr @fn.5
