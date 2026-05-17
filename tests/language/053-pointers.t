use std.io

main :: fn () -> i32 {
    values: [3]i32 = [10, 20, 30]
    array_ptr: ^[3]i32 = ^values
    elem_ptr: ^i32 = ^values[1]
    literal_ptr: ^[3]i32 = ^[1, 2, 3]

    prn($"array pointer = {array_ptr[0]}")
    prn($"elem pointer = {elem_ptr[0]}")
    prn($"literal pointer = {literal_ptr[0]}")

    return array_ptr[0][0] + elem_ptr[0] + literal_ptr[0][2]
}
¬
33
¬
array pointer = [10, 20, 30]
elem pointer = 20
literal pointer = [1, 2, 3]

¬
hir 0
module module.0(053-pointers.input)
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
  let values: [3]i32 = [3]i32 array(i32 10, i32 20, i32 30)
  let array_ptr: ^[3]i32 = ^[3]i32 address_of([3]i32 local.0(values))
  let elem_ptr: ^i32 = ^i32 address_of(i32 index([3]i32 local.0(values), untyped integer 1))
  let literal_ptr: ^[3]i32 = ^[3]i32 address_of([3]i32 array(untyped integer 1, i32 2, i32 3))
  expr void call bind.2(prn)(string interpolate(<unknown> "array pointer = ", [3]i32 index(^[3]i32 local.1(array_ptr), untyped integer 0)))
  expr void call bind.2(prn)(string interpolate(<unknown> "elem pointer = ", i32 index(^i32 local.2(elem_ptr), untyped integer 0)))
  expr void call bind.2(prn)(string interpolate(<unknown> "literal pointer = ", [3]i32 index(^[3]i32 local.3(literal_ptr), untyped integer 0)))
  return i32 add(i32 add(i32 index([3]i32 index(^[3]i32 local.1(array_ptr), untyped integer 0), untyped integer 0), i32 index(^i32 local.2(elem_ptr), untyped integer 0)), i32 index([3]i32 index(^[3]i32 local.3(literal_ptr), untyped integer 0), untyped integer 2))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [17 x i8] c"array pointer = \00"
@.str.m0.1 = private unnamed_addr constant [16 x i8] c"elem pointer = \00"
@.str.m0.2 = private unnamed_addr constant [19 x i8] c"literal pointer = \00"

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

define internal i32 @fn.0() {
  %t0 = insertvalue [3 x i32] poison, i32 10, 0
  %t1 = insertvalue [3 x i32] %t0, i32 20, 1
  %t2 = insertvalue [3 x i32] %t1, i32 30, 2
  %local.0 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.0
  %t3 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 1
  %t4 = insertvalue [3 x i32] poison, i32 1, 0
  %t5 = insertvalue [3 x i32] %t4, i32 2, 1
  %t6 = insertvalue [3 x i32] %t5, i32 3, 2
  %t7 = alloca [3 x i32]
  store [3 x i32] %t6, ptr %t7
  %t8 = call i64 @string_builder_mark()
  %t9 = alloca { ptr, i64 }
  %t10 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.0, i64 16 }, ptr %t10
  call void @to_string$string(ptr %t9, ptr %t10)
  call void @string_builder_append_string(ptr %t9)
  %t11 = getelementptr inbounds [3 x i32], ptr %local.0, i32 0
  %t12 = load [3 x i32], ptr %t11
  call void @string_builder_append_byte(i8 91)
  %t13 = extractvalue [3 x i32] %t12, 0
  %t14 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t14, i32 %t13)
  call void @string_builder_append_string(ptr %t14)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t15 = extractvalue [3 x i32] %t12, 1
  %t16 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t16, i32 %t15)
  call void @string_builder_append_string(ptr %t16)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t17 = extractvalue [3 x i32] %t12, 2
  %t18 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t18, i32 %t17)
  call void @string_builder_append_string(ptr %t18)
  call void @string_builder_append_byte(i8 93)
  %t19 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t19, i64 %t8)
  %t20 = load { ptr, i64 }, ptr %t19
  call void @$prn({ ptr, i64 } %t20)
  %t21 = call i64 @string_builder_mark()
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 15 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = getelementptr inbounds i32, ptr %t3, i32 0
  %t25 = load i32, ptr %t24
  %t26 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t26, i32 %t25)
  call void @string_builder_append_string(ptr %t26)
  %t27 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t27, i64 %t21)
  %t28 = load { ptr, i64 }, ptr %t27
  call void @$prn({ ptr, i64 } %t28)
  %t29 = call i64 @string_builder_mark()
  %t30 = alloca { ptr, i64 }
  %t31 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 18 }, ptr %t31
  call void @to_string$string(ptr %t30, ptr %t31)
  call void @string_builder_append_string(ptr %t30)
  %t32 = getelementptr inbounds [3 x i32], ptr %t7, i32 0
  %t33 = load [3 x i32], ptr %t32
  call void @string_builder_append_byte(i8 91)
  %t34 = extractvalue [3 x i32] %t33, 0
  %t35 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t35, i32 %t34)
  call void @string_builder_append_string(ptr %t35)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t36 = extractvalue [3 x i32] %t33, 1
  %t37 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t37, i32 %t36)
  call void @string_builder_append_string(ptr %t37)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t38 = extractvalue [3 x i32] %t33, 2
  %t39 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t39, i32 %t38)
  call void @string_builder_append_string(ptr %t39)
  call void @string_builder_append_byte(i8 93)
  %t40 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t40, i64 %t29)
  %t41 = load { ptr, i64 }, ptr %t40
  call void @$prn({ ptr, i64 } %t41)
  %t42 = getelementptr inbounds [3 x i32], ptr %local.0, i32 0
  %t43 = load [3 x i32], ptr %t42
  %t44 = extractvalue [3 x i32] %t43, 0
  %t45 = getelementptr inbounds i32, ptr %t3, i32 0
  %t46 = load i32, ptr %t45
  %t47 = add i32 %t44, %t46
  %t48 = getelementptr inbounds [3 x i32], ptr %t7, i32 0
  %t49 = load [3 x i32], ptr %t48
  %t50 = extractvalue [3 x i32] %t49, 2
  %t51 = add i32 %t47, %t50
  ret i32 %t51
}

@$main = alias i32 (), ptr @fn.0
