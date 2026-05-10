use std.io

main :: fn () -> i32 {
    values: [5]i32 = [10, 20, 30, 40, 50]
    all: []i32 = values[..]
    end :: all.count - 1

    prefix: []i32 = values[.. end]
    middle: []i32 = values[1 .. end]
    nested: []i32 = values[.. all.count - 2]

    prn($"{prefix}")
    prn($"{middle}")
    prn($"{nested}")

    return prefix[3] + middle[2] + nested[2]
}
¬
110
¬
[10, 20, 30, 40]
[20, 30, 40]
[10, 20, 30]

¬
hir 0
module module.0(080-dynamic-slice-bounds.input)
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
  let values: [5]i32 = [5]i32 array(i32 10, i32 20, i32 30, i32 40, i32 50)
  let all: []i32 = []i32 slice([5]i32 local.1(values), <none>, <none>)
  let end: usize = usize subtract(usize field([]i32 local.2(all), count), usize 1)
  let prefix: []i32 = []i32 slice([5]i32 local.1(values), <none>, usize local.0(end))
  let middle: []i32 = []i32 slice([5]i32 local.1(values), untyped integer 1, usize local.0(end))
  let nested: []i32 = []i32 slice([5]i32 local.1(values), <none>, usize subtract(usize field([]i32 local.2(all), count), usize 2))
  expr void call bind.2(prn)(string interpolate([]i32 local.3(prefix)))
  expr void call bind.2(prn)(string interpolate([]i32 local.4(middle)))
  expr void call bind.2(prn)(string interpolate([]i32 local.5(nested)))
  return i32 add(i32 add(i32 index([]i32 local.3(prefix), untyped integer 3), i32 index([]i32 local.4(middle), untyped integer 2)), i32 index([]i32 local.5(nested), untyped integer 2))
}
¬
; nerd llvm-ir 0
; generated from HIR

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
  %t0 = insertvalue [5 x i32] poison, i32 10, 0
  %t1 = insertvalue [5 x i32] %t0, i32 20, 1
  %t2 = insertvalue [5 x i32] %t1, i32 30, 2
  %t3 = insertvalue [5 x i32] %t2, i32 40, 3
  %t4 = insertvalue [5 x i32] %t3, i32 50, 4
  %local.1 = alloca [5 x i32]
  store [5 x i32] %t4, ptr %local.1
  %t5 = getelementptr inbounds [5 x i32], ptr %local.1, i64 0, i64 0
  %t6 = insertvalue { ptr, i64 } poison, ptr %t5, 0
  %t7 = insertvalue { ptr, i64 } %t6, i64 5, 1
  %t8 = extractvalue { ptr, i64 } %t7, 1
  %t9 = sub i64 %t8, 1
  %t10 = getelementptr inbounds [5 x i32], ptr %local.1, i64 0, i64 0
  %t11 = insertvalue { ptr, i64 } poison, ptr %t10, 0
  %t12 = insertvalue { ptr, i64 } %t11, i64 %t9, 1
  %t13 = sub i64 %t9, 1
  %t14 = getelementptr inbounds [5 x i32], ptr %local.1, i64 0, i64 1
  %t15 = insertvalue { ptr, i64 } poison, ptr %t14, 0
  %t16 = insertvalue { ptr, i64 } %t15, i64 %t13, 1
  %t17 = extractvalue { ptr, i64 } %t7, 1
  %t18 = sub i64 %t17, 2
  %t19 = getelementptr inbounds [5 x i32], ptr %local.1, i64 0, i64 0
  %t20 = insertvalue { ptr, i64 } poison, ptr %t19, 0
  %t21 = insertvalue { ptr, i64 } %t20, i64 %t18, 1
  %t22 = call i64 @string_builder_mark()
  call void @string_builder_append_byte(i8 91)
  %t23 = extractvalue { ptr, i64 } %t12, 0
  %t24 = extractvalue { ptr, i64 } %t12, 1
  %t25 = alloca i64
  store i64 0, ptr %t25
  br label %slice.string.cond.0
slice.string.cond.0:
  %t26 = load i64, ptr %t25
  %t27 = icmp ult i64 %t26, %t24
  br i1 %t27, label %slice.string.body.1, label %slice.string.end.4
slice.string.body.1:
  %t28 = icmp ne i64 %t26, 0
  br i1 %t28, label %slice.string.sep.2, label %slice.string.item.3
slice.string.sep.2:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.3
slice.string.item.3:
  %t29 = getelementptr inbounds i32, ptr %t23, i64 %t26
  %t30 = load i32, ptr %t29
  %t31 = call { ptr, i64 } @to_string$i32(i32 %t30)
  call void @string_builder_append_string({ ptr, i64 } %t31)
  %t32 = add i64 %t26, 1
  store i64 %t32, ptr %t25
  br label %slice.string.cond.0
slice.string.end.4:
  call void @string_builder_append_byte(i8 93)
  %t33 = call { ptr, i64 } @string_builder_finish(i64 %t22)
  call void @$prn({ ptr, i64 } %t33)
  %t34 = call i64 @string_builder_mark()
  call void @string_builder_append_byte(i8 91)
  %t35 = extractvalue { ptr, i64 } %t16, 0
  %t36 = extractvalue { ptr, i64 } %t16, 1
  %t37 = alloca i64
  store i64 0, ptr %t37
  br label %slice.string.cond.5
slice.string.cond.5:
  %t38 = load i64, ptr %t37
  %t39 = icmp ult i64 %t38, %t36
  br i1 %t39, label %slice.string.body.6, label %slice.string.end.9
slice.string.body.6:
  %t40 = icmp ne i64 %t38, 0
  br i1 %t40, label %slice.string.sep.7, label %slice.string.item.8
slice.string.sep.7:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.8
slice.string.item.8:
  %t41 = getelementptr inbounds i32, ptr %t35, i64 %t38
  %t42 = load i32, ptr %t41
  %t43 = call { ptr, i64 } @to_string$i32(i32 %t42)
  call void @string_builder_append_string({ ptr, i64 } %t43)
  %t44 = add i64 %t38, 1
  store i64 %t44, ptr %t37
  br label %slice.string.cond.5
slice.string.end.9:
  call void @string_builder_append_byte(i8 93)
  %t45 = call { ptr, i64 } @string_builder_finish(i64 %t34)
  call void @$prn({ ptr, i64 } %t45)
  %t46 = call i64 @string_builder_mark()
  call void @string_builder_append_byte(i8 91)
  %t47 = extractvalue { ptr, i64 } %t21, 0
  %t48 = extractvalue { ptr, i64 } %t21, 1
  %t49 = alloca i64
  store i64 0, ptr %t49
  br label %slice.string.cond.10
slice.string.cond.10:
  %t50 = load i64, ptr %t49
  %t51 = icmp ult i64 %t50, %t48
  br i1 %t51, label %slice.string.body.11, label %slice.string.end.14
slice.string.body.11:
  %t52 = icmp ne i64 %t50, 0
  br i1 %t52, label %slice.string.sep.12, label %slice.string.item.13
slice.string.sep.12:
  call void @string_builder_append_byte(i8 44)
  call void @string_builder_append_byte(i8 32)
  br label %slice.string.item.13
slice.string.item.13:
  %t53 = getelementptr inbounds i32, ptr %t47, i64 %t50
  %t54 = load i32, ptr %t53
  %t55 = call { ptr, i64 } @to_string$i32(i32 %t54)
  call void @string_builder_append_string({ ptr, i64 } %t55)
  %t56 = add i64 %t50, 1
  store i64 %t56, ptr %t49
  br label %slice.string.cond.10
slice.string.end.14:
  call void @string_builder_append_byte(i8 93)
  %t57 = call { ptr, i64 } @string_builder_finish(i64 %t46)
  call void @$prn({ ptr, i64 } %t57)
  %t58 = extractvalue { ptr, i64 } %t12, 0
  %t59 = getelementptr inbounds i32, ptr %t58, i32 3
  %t60 = load i32, ptr %t59
  %t61 = extractvalue { ptr, i64 } %t16, 0
  %t62 = getelementptr inbounds i32, ptr %t61, i32 2
  %t63 = load i32, ptr %t62
  %t64 = add i32 %t60, %t63
  %t65 = extractvalue { ptr, i64 } %t21, 0
  %t66 = getelementptr inbounds i32, ptr %t65, i32 2
  %t67 = load i32, ptr %t66
  %t68 = add i32 %t64, %t67
  ret i32 %t68
}

@$main = alias i32 (), ptr @fn.0
