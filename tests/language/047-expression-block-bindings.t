use std.io

-- Uses expression blocks as top-level constant and variable initialisers.
constant_value :: ${
    break 4
}

variable_value := ${
    break constant_value + 6
}

main :: fn () {
    prn($"constant = {constant_value}")
    prn($"variable = {variable_value}")

    variable_value += 2
    prn($"updated = {variable_value}")

    return variable_value
}
¬
12
¬
constant = 4
variable = 10
updated = 12

¬
hir 0
module module.0(047-expression-block-bindings.input)
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
bind constant_value = value.0
bind variable_value = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer block {
    break untyped integer 4
  }
global value.1: i32 = untyped integer block {
    break untyped integer add(untyped integer bind.5(constant_value), untyped integer 6)
  }
func fn.0() -> i32 {
  expr void call bind.2(prn)(string interpolate(<unknown> "constant = ", untyped integer bind.5(constant_value)))
  expr void call bind.2(prn)(string interpolate(<unknown> "variable = ", i32 bind.6(variable_value)))
  assign i32 bind.6(variable_value) = i32 add(i32 bind.6(variable_value), i32 2)
  expr void call bind.2(prn)(string interpolate(<unknown> "updated = ", i32 bind.6(variable_value)))
  return i32 bind.6(variable_value)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [12 x i8] c"constant = \00"
@.str.m0.1 = private unnamed_addr constant [12 x i8] c"variable = \00"
@.str.m0.2 = private unnamed_addr constant [11 x i8] c"updated = \00"

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

@$variable_value = global i32 0

define void @m0.init() {
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  %t1 = alloca i32, align 4
  store i32 0, ptr %t1, align 4
  store i32 4, ptr %t1, align 4
  br label %block.end.1
block.end.1:
  %t2 = load i32, ptr %t1, align 4
  %t3 = add i32 %t2, 6
  store i32 %t3, ptr %t0, align 4
  br label %block.end.0
block.end.0:
  %t4 = load i32, ptr %t0, align 4
  store i32 %t4, ptr @$variable_value
  ret void
}

define i32 @fn.0() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t1)
  %t2 = alloca i32, align 4
  store i32 0, ptr %t2, align 4
  store i32 4, ptr %t2, align 4
  br label %block.end.0
block.end.0:
  %t3 = load i32, ptr %t2, align 4
  %t4 = call { ptr, i64 } @to_string$i32(i32 %t3)
  call void @string_builder_append_string({ ptr, i64 } %t4)
  %t5 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t5)
  %t6 = call i64 @string_builder_mark()
  %t7 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 11 })
  call void @string_builder_append_string({ ptr, i64 } %t7)
  %t8 = load i32, ptr @$variable_value
  %t9 = call { ptr, i64 } @to_string$i32(i32 %t8)
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = call { ptr, i64 } @string_builder_finish(i64 %t6)
  call void @$prn({ ptr, i64 } %t10)
  %t11 = load i32, ptr @$variable_value
  %t12 = add i32 %t11, 2
  store i32 %t12, ptr @$variable_value
  %t13 = call i64 @string_builder_mark()
  %t14 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 10 })
  call void @string_builder_append_string({ ptr, i64 } %t14)
  %t15 = load i32, ptr @$variable_value
  %t16 = call { ptr, i64 } @to_string$i32(i32 %t15)
  call void @string_builder_append_string({ ptr, i64 } %t16)
  %t17 = call { ptr, i64 } @string_builder_finish(i64 %t13)
  call void @$prn({ ptr, i64 } %t17)
  %t18 = load i32, ptr @$variable_value
  ret i32 %t18
}

@$main = alias i32 (), ptr @fn.0
