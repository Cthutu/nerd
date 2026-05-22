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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind constant_value = value.0
bind variable_value = value.1
bind main = fn.0
const value.0: untyped integer = untyped integer block {
    break untyped integer 4
  }
global value.1: i32 = untyped integer block {
    break untyped integer add(untyped integer bind.2(constant_value), untyped integer 6)
  }
func fn.0() -> i32 {
  expr void call bind.0(prn)(string interpolate(<unknown> "constant = ", untyped integer bind.2(constant_value)))
  expr void call bind.0(prn)(string interpolate(<unknown> "variable = ", i32 bind.3(variable_value)))
  assign i32 bind.3(variable_value) = i32 add(i32 bind.3(variable_value), i32 2)
  expr void call bind.0(prn)(string interpolate(<unknown> "updated = ", i32 bind.3(variable_value)))
  return i32 bind.3(variable_value)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [47 x i8] c"tests/language/047-expression-block-bindings.t\00"
@.str.m0.0 = private unnamed_addr constant [12 x i8] c"constant = \00"
@.str.m0.1 = private unnamed_addr constant [12 x i8] c"variable = \00"
@.str.m0.2 = private unnamed_addr constant [11 x i8] c"updated = \00"

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

@$variable_value = internal global i32 0

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

define internal i32 @fn.0() {
  %t2 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  %t19 = alloca { ptr, i64 }
  %t0 = call i64 @string_builder_mark()
  %t1 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 11 }, ptr %t2
  call void @to_string$string(ptr %t1, ptr %t2)
  call void @string_builder_append_string(ptr %t1)
  %t3 = alloca i32, align 4
  store i32 0, ptr %t3, align 4
  store i32 4, ptr %t3, align 4
  br label %block.end.0
block.end.0:
  %t4 = load i32, ptr %t3, align 4
  %t5 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t5, i32 %t4)
  call void @string_builder_append_string(ptr %t5)
  %t6 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t6, i64 %t0)
  %t7 = load { ptr, i64 }, ptr %t6
  call void @$prn({ ptr, i64 } %t7)
  %t8 = call i64 @string_builder_mark()
  %t9 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 11 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = load i32, ptr @$variable_value
  %t12 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t12, i32 %t11)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t13, i64 %t8)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn({ ptr, i64 } %t14)
  %t15 = load i32, ptr @$variable_value
  %t16 = add i32 %t15, 2
  store i32 %t16, ptr @$variable_value
  %t17 = call i64 @string_builder_mark()
  %t18 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 10 }, ptr %t19
  call void @to_string$string(ptr %t18, ptr %t19)
  call void @string_builder_append_string(ptr %t18)
  %t20 = load i32, ptr @$variable_value
  %t21 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t21, i32 %t20)
  call void @string_builder_append_string(ptr %t21)
  %t22 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t22, i64 %t17)
  %t23 = load { ptr, i64 }, ptr %t22
  call void @$prn({ ptr, i64 } %t23)
  %t24 = load i32, ptr @$variable_value
  ret i32 %t24
}

@$main = alias i32 (), ptr @fn.0
