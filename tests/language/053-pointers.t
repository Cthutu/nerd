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

define i32 @fn.0() {
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
  %t9 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.0, i64 16 })
  call void @string_builder_append_string({ ptr, i64 } %t9)
  %t10 = getelementptr inbounds [3 x i32], ptr %local.0, i32 0
  %t11 = load [3 x i32], ptr %t10
  call void @string_builder_append_byte(i8 91)
  %t12 = extractvalue [3 x i32] %t11, 0
  %t13 = call { ptr, i64 } @to_string$i32(i32 %t12)
  call void @string_builder_append_string({ ptr, i64 } %t13)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t14 = extractvalue [3 x i32] %t11, 1
  %t15 = call { ptr, i64 } @to_string$i32(i32 %t14)
  call void @string_builder_append_string({ ptr, i64 } %t15)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t16 = extractvalue [3 x i32] %t11, 2
  %t17 = call { ptr, i64 } @to_string$i32(i32 %t16)
  call void @string_builder_append_string({ ptr, i64 } %t17)
  call void @string_builder_append_byte(i8 93)
  %t18 = call { ptr, i64 } @string_builder_finish(i64 %t8)
  call void @$prn({ ptr, i64 } %t18)
  %t19 = call i64 @string_builder_mark()
  %t20 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 15 })
  call void @string_builder_append_string({ ptr, i64 } %t20)
  %t21 = getelementptr inbounds i32, ptr %t3, i32 0
  %t22 = load i32, ptr %t21
  %t23 = call { ptr, i64 } @to_string$i32(i32 %t22)
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = call { ptr, i64 } @string_builder_finish(i64 %t19)
  call void @$prn({ ptr, i64 } %t24)
  %t25 = call i64 @string_builder_mark()
  %t26 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 18 })
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = getelementptr inbounds [3 x i32], ptr %t7, i32 0
  %t28 = load [3 x i32], ptr %t27
  call void @string_builder_append_byte(i8 91)
  %t29 = extractvalue [3 x i32] %t28, 0
  %t30 = call { ptr, i64 } @to_string$i32(i32 %t29)
  call void @string_builder_append_string({ ptr, i64 } %t30)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t31 = extractvalue [3 x i32] %t28, 1
  %t32 = call { ptr, i64 } @to_string$i32(i32 %t31)
  call void @string_builder_append_string({ ptr, i64 } %t32)
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  %t33 = extractvalue [3 x i32] %t28, 2
  %t34 = call { ptr, i64 } @to_string$i32(i32 %t33)
  call void @string_builder_append_string({ ptr, i64 } %t34)
  call void @string_builder_append_byte(i8 93)
  %t35 = call { ptr, i64 } @string_builder_finish(i64 %t25)
  call void @$prn({ ptr, i64 } %t35)
  %t36 = getelementptr inbounds [3 x i32], ptr %local.0, i32 0
  %t37 = load [3 x i32], ptr %t36
  %t38 = extractvalue [3 x i32] %t37, 0
  %t39 = getelementptr inbounds i32, ptr %t3, i32 0
  %t40 = load i32, ptr %t39
  %t41 = add i32 %t38, %t40
  %t42 = getelementptr inbounds [3 x i32], ptr %t7, i32 0
  %t43 = load [3 x i32], ptr %t42
  %t44 = extractvalue [3 x i32] %t43, 2
  %t45 = add i32 %t41, %t44
  ret i32 %t45
}

@$main = alias i32 (), ptr @fn.0
