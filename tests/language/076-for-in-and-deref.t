use std.io

main :: fn () -> i32 {
    values: [3]i32 = [1, 2, 3]

    for _item in values[..] {
    }

    total := 0
    for item in values[..] {
        total += item^
    }

    text := "ab"
    count := 0
    sum := 0
    for ch in text {
        count += 1
        sum += ch^.as(i32)
    }

    ptr: ^i32 = ^values[1]
    prn($"{ptr^} {total} {count} {sum}")
    return ptr^ + total + count
}
¬
10
¬
2 6 2 195

¬
hir 0
module module.0(076-for-in-and-deref.input)
import module.1(std.io)
import import.0 pr from module.1(std.io).decl.6: fn (string) -> void
import import.1 epr from module.1(std.io).decl.7: fn (string) -> void
import import.2 prn from module.1(std.io).decl.8: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.9: fn (string) -> void
import import.4 input from module.1(std.io).decl.10: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
bind main = fn.0
func fn.0() -> i32 {
  let values: [3]i32 = [3]i32 array(i32 1, i32 2, i32 3)
  expr void for in _item: ^i32 in []i32 slice([3]i32 local.0(values), <none>, <none>) {
    body {
    }
  }
  let total: i32 = untyped integer 0
  expr void for in item: ^i32 in []i32 slice([3]i32 local.0(values), <none>, <none>) {
    body {
      assign i32 local.2(total) = i32 add(i32 local.2(total), i32 deref(^i32 local.3(item)))
    }
  }
  let text: string = string "ab"
  let count: i32 = untyped integer 0
  let sum: i32 = untyped integer 0
  expr void for in ch: ^u8 in string local.4(text) {
    body {
      assign i32 local.5(count) = i32 add(i32 local.5(count), i32 1)
      assign i32 local.6(sum) = i32 add(i32 local.6(sum), i32 cast(u8 deref(^u8 local.7(ch)) as i32))
    }
  }
  let ptr: ^i32 = ^i32 address_of(i32 index([3]i32 local.0(values), untyped integer 1))
  expr void call bind.2(prn)(string interpolate(i32 deref(^i32 local.8(ptr)), <unknown> " ", i32 local.2(total), <unknown> " ", i32 local.5(count), <unknown> " ", i32 local.6(sum)))
  return i32 add(i32 add(i32 deref(^i32 local.8(ptr)), i32 local.2(total)), i32 local.5(count))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [3 x i8] c"ab\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"

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
  %t0 = insertvalue [3 x i32] poison, i32 1, 0
  %t1 = insertvalue [3 x i32] %t0, i32 2, 1
  %t2 = insertvalue [3 x i32] %t1, i32 3, 2
  %local.0 = alloca [3 x i32]
  store [3 x i32] %t2, ptr %local.0
  %t3 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i64 0
  %t4 = insertvalue { ptr, i64 } poison, ptr %t3, 0
  %t5 = insertvalue { ptr, i64 } %t4, i64 3, 1
  %t6 = extractvalue { ptr, i64 } %t5, 0
  %t7 = extractvalue { ptr, i64 } %t5, 1
  %t8 = alloca i64
  store i64 0, ptr %t8
  %local.1 = alloca ptr
  br label %for.in.cond.0
for.in.cond.0:
  %t9 = load i64, ptr %t8
  %t10 = icmp ult i64 %t9, %t7
  br i1 %t10, label %for.in.body.1, label %for.in.end.2
for.in.body.1:
  %t11 = getelementptr inbounds i32, ptr %t6, i64 %t9
  store ptr %t11, ptr %local.1
  %t12 = load i64, ptr %t8
  %t13 = add i64 %t12, 1
  store i64 %t13, ptr %t8
  br label %for.in.cond.0
for.in.end.2:
  %local.2 = alloca i32
  store i32 0, ptr %local.2
  %t14 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i64 0
  %t15 = insertvalue { ptr, i64 } poison, ptr %t14, 0
  %t16 = insertvalue { ptr, i64 } %t15, i64 3, 1
  %t17 = extractvalue { ptr, i64 } %t16, 0
  %t18 = extractvalue { ptr, i64 } %t16, 1
  %t19 = alloca i64
  store i64 0, ptr %t19
  %local.3 = alloca ptr
  br label %for.in.cond.3
for.in.cond.3:
  %t20 = load i64, ptr %t19
  %t21 = icmp ult i64 %t20, %t18
  br i1 %t21, label %for.in.body.4, label %for.in.end.5
for.in.body.4:
  %t22 = getelementptr inbounds i32, ptr %t17, i64 %t20
  store ptr %t22, ptr %local.3
  %t23 = load i32, ptr %local.2
  %t24 = load ptr, ptr %local.3
  %t25 = load i32, ptr %t24
  %t26 = add i32 %t23, %t25
  store i32 %t26, ptr %local.2
  %t27 = load i64, ptr %t19
  %t28 = add i64 %t27, 1
  store i64 %t28, ptr %t19
  br label %for.in.cond.3
for.in.end.5:
  %local.5 = alloca i32
  store i32 0, ptr %local.5
  %local.6 = alloca i32
  store i32 0, ptr %local.6
  %t29 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 2 }, 0
  %t30 = extractvalue { ptr, i64 } { ptr @.str.m0.0, i64 2 }, 1
  %t31 = alloca i64
  store i64 0, ptr %t31
  %local.7 = alloca ptr
  br label %for.in.cond.6
for.in.cond.6:
  %t32 = load i64, ptr %t31
  %t33 = icmp ult i64 %t32, %t30
  br i1 %t33, label %for.in.body.7, label %for.in.end.8
for.in.body.7:
  %t34 = getelementptr inbounds i8, ptr %t29, i64 %t32
  store ptr %t34, ptr %local.7
  %t35 = load i32, ptr %local.5
  %t36 = add i32 %t35, 1
  store i32 %t36, ptr %local.5
  %t37 = load i32, ptr %local.6
  %t38 = load ptr, ptr %local.7
  %t39 = load i8, ptr %t38
  %t40 = zext i8 %t39 to i32
  %t41 = add i32 %t37, %t40
  store i32 %t41, ptr %local.6
  %t42 = load i64, ptr %t31
  %t43 = add i64 %t42, 1
  store i64 %t43, ptr %t31
  br label %for.in.cond.6
for.in.end.8:
  %t44 = getelementptr inbounds [3 x i32], ptr %local.0, i64 0, i32 1
  %t45 = call i64 @string_builder_mark()
  %t46 = load i32, ptr %t44
  %t47 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t47, i32 %t46)
  call void @string_builder_append_string(ptr %t47)
  %t48 = alloca { ptr, i64 }
  %t49 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 1 }, ptr %t49
  call void @to_string$string(ptr %t48, ptr %t49)
  call void @string_builder_append_string(ptr %t48)
  %t50 = load i32, ptr %local.2
  %t51 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t51, i32 %t50)
  call void @string_builder_append_string(ptr %t51)
  %t52 = alloca { ptr, i64 }
  %t53 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t53
  call void @to_string$string(ptr %t52, ptr %t53)
  call void @string_builder_append_string(ptr %t52)
  %t54 = load i32, ptr %local.5
  %t55 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t55, i32 %t54)
  call void @string_builder_append_string(ptr %t55)
  %t56 = alloca { ptr, i64 }
  %t57 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t57
  call void @to_string$string(ptr %t56, ptr %t57)
  call void @string_builder_append_string(ptr %t56)
  %t58 = load i32, ptr %local.6
  %t59 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t59, i32 %t58)
  call void @string_builder_append_string(ptr %t59)
  %t60 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t60, i64 %t45)
  %t61 = load { ptr, i64 }, ptr %t60
  call void @$prn({ ptr, i64 } %t61)
  %t62 = load i32, ptr %t44
  %t63 = load i32, ptr %local.2
  %t64 = add i32 %t62, %t63
  %t65 = load i32, ptr %local.5
  %t66 = add i32 %t64, %t65
  ret i32 %t66
}

@$main = alias i32 (), ptr @fn.0
