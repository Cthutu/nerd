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
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 8 }, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = alloca { ptr, i64 }
  %t25 = alloca { ptr, i64 }
  store { ptr, i64 } %t4, ptr %t25
  call void @to_string$string(ptr %t24, ptr %t25)
  call void @string_builder_append_string(ptr %t24)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t21)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  %t28 = call i64 @string_builder_mark()
  %t29 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 6 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = alloca { ptr, i64 }
  %t32 = alloca { ptr, i64 }
  store { ptr, i64 } %t9, ptr %t32
  call void @to_string$string(ptr %t31, ptr %t32)
  call void @string_builder_append_string(ptr %t31)
  %t33 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t33, i64 %t28)
  %t34 = load { ptr, i64 }, ptr %t33
  call void @$prn({ ptr, i64 } %t34)
  %t35 = call i64 @string_builder_mark()
  %t36 = alloca { ptr, i64 }
  %t37 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 7 }, ptr %t37
  call void @to_string$string(ptr %t36, ptr %t37)
  call void @string_builder_append_string(ptr %t36)
  %t38 = alloca { ptr, i64 }
  %t39 = alloca { ptr, i64 }
  store { ptr, i64 } %t15, ptr %t39
  call void @to_string$string(ptr %t38, ptr %t39)
  call void @string_builder_append_string(ptr %t38)
  %t40 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t40, i64 %t35)
  %t41 = load { ptr, i64 }, ptr %t40
  call void @$prn({ ptr, i64 } %t41)
  %t42 = call i64 @string_builder_mark()
  %t43 = alloca { ptr, i64 }
  %t44 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t44
  call void @to_string$string(ptr %t43, ptr %t44)
  call void @string_builder_append_string(ptr %t43)
  %t45 = alloca { ptr, i64 }
  %t46 = alloca { ptr, i64 }
  store { ptr, i64 } %t20, ptr %t46
  call void @to_string$string(ptr %t45, ptr %t46)
  call void @string_builder_append_string(ptr %t45)
  %t47 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t47, i64 %t42)
  %t48 = load { ptr, i64 }, ptr %t47
  call void @$prn({ ptr, i64 } %t48)
  %t49 = call i64 @string_builder_mark()
  %t50 = alloca { ptr, i64 }
  %t51 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.5, i64 8 }, ptr %t51
  call void @to_string$string(ptr %t50, ptr %t51)
  call void @string_builder_append_string(ptr %t50)
  %t52 = extractvalue { ptr, i64 } %t9, 1
  %t53 = alloca { ptr, i64 }
  call void @to_string$usize(ptr %t53, i64 %t52)
  call void @string_builder_append_string(ptr %t53)
  %t54 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t54, i64 %t49)
  %t55 = load { ptr, i64 }, ptr %t54
  call void @$prn({ ptr, i64 } %t55)
  %t56 = call i64 @string_builder_mark()
  %t57 = alloca { ptr, i64 }
  %t58 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.6, i64 13 }, ptr %t58
  call void @to_string$string(ptr %t57, ptr %t58)
  call void @string_builder_append_string(ptr %t57)
  %t59 = extractvalue { ptr, i64 } %t9, 0
  %t60 = getelementptr inbounds i8, ptr %t59, i32 0
  %t61 = load i8, ptr %t60
  %t62 = alloca { ptr, i64 }
  call void @to_string$u8(ptr %t62, i8 %t61)
  call void @string_builder_append_string(ptr %t62)
  %t63 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t63, i64 %t56)
  %t64 = load { ptr, i64 }, ptr %t63
  call void @$prn({ ptr, i64 } %t64)
  %t65 = alloca { ptr, i64 }
  store { ptr, i64 } %t9, ptr %t65
  %t66 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.7, i64 3 }, ptr %t66
  %t67 = call i1 @string_eq(ptr %t65, ptr %t66)
  br i1 %t67, label %on.body.1, label %on.next.2
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
  %t68 = phi i32 [7, %on.value.3], [1, %on.value.6]
  %t69 = extractvalue { ptr, i64 } %t9, 0
  %t70 = getelementptr inbounds i8, ptr %t69, i32 1
  %t71 = load i8, ptr %t70
  %t72 = zext i8 %t71 to i32
  %t73 = add i32 %t68, %t72
  ret i32 %t73
}

@$main = alias i32 (), ptr @fn.0
