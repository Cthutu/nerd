use std.io

main :: fn () {
    n := 1
    n += 2
    n *= 4
    n -= 5
    n /= 2
    n %= 2

    bits := 6
    bits &= 3
    bits ^= 7
    bits |= 8

    flag := yes
    flag &&= no
    flag ||= yes

    prn($"{n} {bits} {flag}")
}
¬
0
¬
1 13 yes

¬
hir 0
module module.0(040-compound-assignments.input)
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
  let n: i32 = untyped integer 1
  assign i32 local.0(n) = i32 add(i32 local.0(n), i32 2)
  assign i32 local.0(n) = i32 multiply(i32 local.0(n), i32 4)
  assign i32 local.0(n) = i32 subtract(i32 local.0(n), i32 5)
  assign i32 local.0(n) = i32 divide(i32 local.0(n), i32 2)
  assign i32 local.0(n) = i32 modulo(i32 local.0(n), i32 2)
  let bits: i32 = untyped integer 6
  assign i32 local.1(bits) = i32 bitwise_and(i32 local.1(bits), i32 3)
  assign i32 local.1(bits) = i32 bitwise_xor(i32 local.1(bits), i32 7)
  assign i32 local.1(bits) = i32 bitwise_or(i32 local.1(bits), i32 8)
  let flag: bool = bool yes
  assign bool local.2(flag) = bool logical_and(bool local.2(flag), bool no)
  assign bool local.2(flag) = bool logical_or(bool local.2(flag), bool yes)
  expr void call bind.2(prn)(string interpolate(i32 local.0(n), <unknown> " ", i32 local.1(bits), <unknown> " ", bool local.2(flag)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"

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

define void @fn.0() {
  %local.0 = alloca i32
  store i32 1, ptr %local.0
  %t0 = load i32, ptr %local.0
  %t1 = add i32 %t0, 2
  store i32 %t1, ptr %local.0
  %t2 = load i32, ptr %local.0
  %t3 = mul i32 %t2, 4
  store i32 %t3, ptr %local.0
  %t4 = load i32, ptr %local.0
  %t5 = sub i32 %t4, 5
  store i32 %t5, ptr %local.0
  %t6 = load i32, ptr %local.0
  %t7 = sdiv i32 %t6, 2
  store i32 %t7, ptr %local.0
  %t8 = load i32, ptr %local.0
  %t9 = srem i32 %t8, 2
  store i32 %t9, ptr %local.0
  %local.1 = alloca i32
  store i32 6, ptr %local.1
  %t10 = load i32, ptr %local.1
  %t11 = and i32 %t10, 3
  store i32 %t11, ptr %local.1
  %t12 = load i32, ptr %local.1
  %t13 = xor i32 %t12, 7
  store i32 %t13, ptr %local.1
  %t14 = load i32, ptr %local.1
  %t15 = or i32 %t14, 8
  store i32 %t15, ptr %local.1
  %local.2 = alloca i1
  store i1 1, ptr %local.2
  %t16 = load i1, ptr %local.2
  %t17 = and i1 %t16, 0
  store i1 %t17, ptr %local.2
  %t18 = load i1, ptr %local.2
  %t19 = or i1 %t18, 1
  store i1 %t19, ptr %local.2
  %t20 = call i64 @string_builder_mark()
  %t21 = load i32, ptr %local.0
  %t22 = call { ptr, i64 } @to_string$i32(i32 %t21)
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = load i32, ptr %local.1
  %t25 = call { ptr, i64 } @to_string$i32(i32 %t24)
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = load i1, ptr %local.2
  %t28 = call { ptr, i64 } @to_string$bool(i1 %t27)
  call void @string_builder_append_string({ ptr, i64 } %t28)
  %t29 = call { ptr, i64 } @string_builder_finish(i64 %t20)
  call void @$prn({ ptr, i64 } %t29)
  ret void
}

@$main = alias void (), ptr @fn.0
