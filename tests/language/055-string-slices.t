use std.io

main :: fn () -> i32 {
    text :: "hello"
    whole: string = text[..]
    ell: string = text[1..4]
    tail: string = text[2..]
    head: string = text[..2]

    prn($"whole = {whole}")
    prn($"ell = {ell}")
    prn($"tail = {tail}")
    prn($"head = {head}")
    prn($"count = {ell.count}")
    prn($"first byte = {ell.data[0]}")

    result :: on ell {
        "ell" => 7
        else => 1
    }

    return result + ell.data[1].as(i32)
}
¬
115
¬
whole = hello
ell = ell
tail = llo
head = he
count = 3
first byte = 101

¬
hir 0
module module.0(055-string-slices.input)
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
  let text: string = string "hello"
  let whole: string = string slice(string local.0(text), <none>, <none>)
  let ell: string = string slice(string local.0(text), untyped integer 1, untyped integer 4)
  let tail: string = string slice(string local.0(text), untyped integer 2, <none>)
  let head: string = string slice(string local.0(text), <none>, untyped integer 2)
  expr void call bind.2(prn)(string interpolate(<unknown> "whole = ", string local.2(whole)))
  expr void call bind.2(prn)(string interpolate(<unknown> "ell = ", string local.3(ell)))
  expr void call bind.2(prn)(string interpolate(<unknown> "tail = ", string local.4(tail)))
  expr void call bind.2(prn)(string interpolate(<unknown> "head = ", string local.5(head)))
  expr void call bind.2(prn)(string interpolate(<unknown> "count = ", usize field(string local.3(ell), count)))
  expr void call bind.2(prn)(string interpolate(<unknown> "first byte = ", u8 index(^u8 field(string local.3(ell), data), untyped integer 0)))
  let result: untyped integer = untyped integer on string local.3(ell) {
    value(string "ell") => {
      expr untyped integer 7
    }
    else => {
      expr untyped integer 1
    }
  }
  return i32 add(i32 local.1(result), i32 cast(u8 index(^u8 field(string local.3(ell), data), untyped integer 1) as i32))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"whole = \00"
@.str.m0.2 = private unnamed_addr constant [7 x i8] c"ell = \00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c"tail = \00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"head = \00"
@.str.m0.5 = private unnamed_addr constant [9 x i8] c"count = \00"
@.str.m0.6 = private unnamed_addr constant [14 x i8] c"first byte = \00"
@.str.m0.7 = private unnamed_addr constant [4 x i8] c"ell\00"

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
  %t0 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t1 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 1
  %t2 = getelementptr inbounds i8, ptr %t0, i64 0
  %t3 = insertvalue { ptr, i64 } poison, ptr %t2, 0
  %t4 = insertvalue { ptr, i64 } %t3, i64 %t1, 1
  %t5 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t6 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 1
  %t7 = getelementptr inbounds i8, ptr %t5, i64 1
  %t8 = insertvalue { ptr, i64 } poison, ptr %t7, 0
  %t9 = insertvalue { ptr, i64 } %t8, i64 3, 1
  %t10 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t11 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 1
  %t12 = sub i64 %t11, 2
  %t13 = getelementptr inbounds i8, ptr %t10, i64 2
  %t14 = insertvalue { ptr, i64 } poison, ptr %t13, 0
  %t15 = insertvalue { ptr, i64 } %t14, i64 %t12, 1
  %t16 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t17 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 1
  %t18 = getelementptr inbounds i8, ptr %t16, i64 0
  %t19 = insertvalue { ptr, i64 } poison, ptr %t18, 0
  %t20 = insertvalue { ptr, i64 } %t19, i64 2, 1
  %t21 = call i64 @string_builder_mark()
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t4)
  call void @string_builder_append_string({ ptr, i64 } %t23)
  %t24 = call { ptr, i64 } @string_builder_finish(i64 %t21)
  call void @$prn({ ptr, i64 } %t24)
  %t25 = call i64 @string_builder_mark()
  %t26 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 6 })
  call void @string_builder_append_string({ ptr, i64 } %t26)
  %t27 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t9)
  call void @string_builder_append_string({ ptr, i64 } %t27)
  %t28 = call { ptr, i64 } @string_builder_finish(i64 %t25)
  call void @$prn({ ptr, i64 } %t28)
  %t29 = call i64 @string_builder_mark()
  %t30 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t30)
  %t31 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t15)
  call void @string_builder_append_string({ ptr, i64 } %t31)
  %t32 = call { ptr, i64 } @string_builder_finish(i64 %t29)
  call void @$prn({ ptr, i64 } %t32)
  %t33 = call i64 @string_builder_mark()
  %t34 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t34)
  %t35 = call { ptr, i64 } @to_string$string({ ptr, i64 } %t20)
  call void @string_builder_append_string({ ptr, i64 } %t35)
  %t36 = call { ptr, i64 } @string_builder_finish(i64 %t33)
  call void @$prn({ ptr, i64 } %t36)
  %t37 = call i64 @string_builder_mark()
  %t38 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.5, i64 8 })
  call void @string_builder_append_string({ ptr, i64 } %t38)
  %t39 = extractvalue { ptr, i64 } %t9, 1
  %t40 = call { ptr, i64 } @to_string$usize(i64 %t39)
  call void @string_builder_append_string({ ptr, i64 } %t40)
  %t41 = call { ptr, i64 } @string_builder_finish(i64 %t37)
  call void @$prn({ ptr, i64 } %t41)
  %t42 = call i64 @string_builder_mark()
  %t43 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.6, i64 13 })
  call void @string_builder_append_string({ ptr, i64 } %t43)
  %t44 = extractvalue { ptr, i64 } %t9, 0
  %t45 = getelementptr inbounds i8, ptr %t44, i32 0
  %t46 = load i8, ptr %t45
  %t47 = call { ptr, i64 } @to_string$u8(i8 %t46)
  call void @string_builder_append_string({ ptr, i64 } %t47)
  %t48 = call { ptr, i64 } @string_builder_finish(i64 %t42)
  call void @$prn({ ptr, i64 } %t48)
  %t49 = call i1 @string_eq({ ptr, i64 } %t9, { ptr, i64 } { ptr @.str.m0.7, i64 3 })
  br i1 %t49, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t50 = phi i32 [7, %on.value.3], [1, %on.value.6]
  %t51 = extractvalue { ptr, i64 } %t9, 0
  %t52 = getelementptr inbounds i8, ptr %t51, i32 1
  %t53 = load i8, ptr %t52
  %t54 = zext i8 %t53 to i32
  %t55 = add i32 %t50, %t54
  ret i32 %t55
}

@$main = alias i32 (), ptr @fn.0
