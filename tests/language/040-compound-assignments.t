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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
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
  expr void call bind.0(prn)(string interpolate(i32 local.0(n), <unknown> " ", i32 local.1(bits), <unknown> " ", bool local.2(flag)))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [42 x i8] c"tests/language/040-compound-assignments.t\00"
@.str.m0.0 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"

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
  %local.1 = alloca i32
  %local.2 = alloca i1
  %t26 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
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
  store i1 1, ptr %local.2
  %t16 = load i1, ptr %local.2
  %t17 = alloca i1
  br i1 %t16, label %logical.rhs.0, label %logical.short.1
logical.short.1:
  store i1 0, ptr %t17
  br label %logical.end.2
logical.rhs.0:
  store i1 0, ptr %t17
  br label %logical.end.2
logical.end.2:
  %t18 = load i1, ptr %t17
  store i1 %t18, ptr %local.2
  %t19 = load i1, ptr %local.2
  %t20 = alloca i1
  br i1 %t19, label %logical.short.4, label %logical.rhs.3
logical.short.4:
  store i1 1, ptr %t20
  br label %logical.end.5
logical.rhs.3:
  store i1 1, ptr %t20
  br label %logical.end.5
logical.end.5:
  %t21 = load i1, ptr %t20
  store i1 %t21, ptr %local.2
  %t22 = call i64 @string_builder_mark()
  %t23 = load i32, ptr %local.0
  %t24 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t24, i32 %t23)
  call void @string_builder_append_string(ptr %t24)
  %t25 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 1 }, ptr %t26
  call void @to_string$string(ptr %t25, ptr %t26)
  call void @string_builder_append_string(ptr %t25)
  %t27 = load i32, ptr %local.1
  %t28 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t28, i32 %t27)
  call void @string_builder_append_string(ptr %t28)
  %t29 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = load i1, ptr %local.2
  %t32 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t32, i1 zeroext %t31)
  call void @string_builder_append_string(ptr %t32)
  %t33 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t33, i64 %t22)
  %t34 = load { ptr, i64 }, ptr %t33
  call void @$prn({ ptr, i64 } %t34)
  ret void
}

@$main = alias void (), ptr @fn.0

