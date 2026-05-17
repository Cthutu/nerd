use std.io

main :: fn () {
    first :: ${
        break 7
    }
    second :: ${
        break first + 5
    }
    word :: ${
        break "done"
    }
    flag :: ${
        break yes
    }

    pr("first = ")
    prn($"{first}")
    prn($"second = {second}")
    prn($"word = {word}")
    prn($"flag = {flag}")
    return second
}
¬
12
¬
first = 7
second = 12
word = done
flag = yes

¬
hir 0
module module.0(045-expression-block.input)
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
  let first: untyped integer = untyped integer block {
    break untyped integer 7
  }
  let second: untyped integer = untyped integer block {
    break untyped integer add(untyped integer local.0(first), untyped integer 5)
  }
  let word: string = string block {
    break string "done"
  }
  let flag: bool = bool block {
    break bool yes
  }
  expr void call bind.0(pr)(string "first = ")
  expr void call bind.2(prn)(string interpolate(untyped integer local.0(first)))
  expr void call bind.2(prn)(string interpolate(<unknown> "second = ", untyped integer local.1(second)))
  expr void call bind.2(prn)(string interpolate(<unknown> "word = ", string local.2(word)))
  expr void call bind.2(prn)(string interpolate(<unknown> "flag = ", bool local.3(flag)))
  return untyped integer local.1(second)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"done\00"
@.str.m0.1 = private unnamed_addr constant [9 x i8] c"first = \00"
@.str.m0.2 = private unnamed_addr constant [10 x i8] c"second = \00"
@.str.m0.3 = private unnamed_addr constant [8 x i8] c"word = \00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"flag = \00"

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
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  store i32 7, ptr %t0, align 4
  br label %block.end.0
block.end.0:
  %t1 = load i32, ptr %t0, align 4
  %t2 = alloca i32, align 4
  store i32 0, ptr %t2, align 4
  %t3 = add i32 %t1, 5
  store i32 %t3, ptr %t2, align 4
  br label %block.end.1
block.end.1:
  %t4 = load i32, ptr %t2, align 4
  %t5 = alloca { ptr, i64 }, align 4
  store { ptr, i64 } zeroinitializer, ptr %t5, align 4
  store { ptr, i64 } { ptr @.str.m0.0, i64 4 }, ptr %t5, align 4
  br label %block.end.2
block.end.2:
  %t6 = load { ptr, i64 }, ptr %t5, align 4
  %t7 = alloca i1, align 4
  store i1 0, ptr %t7, align 4
  store i1 1, ptr %t7, align 4
  br label %block.end.3
block.end.3:
  %t8 = load i1, ptr %t7, align 4
  call void @$pr({ ptr, i64 } { ptr @.str.m0.1, i64 8 })
  %t9 = call i64 @string_builder_mark()
  %t10 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t10, i32 %t1)
  call void @string_builder_append_string(ptr %t10)
  %t11 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t11, i64 %t9)
  %t12 = load { ptr, i64 }, ptr %t11
  call void @$prn({ ptr, i64 } %t12)
  %t13 = call i64 @string_builder_mark()
  %t14 = alloca { ptr, i64 }
  %t15 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 9 }, ptr %t15
  call void @to_string$string(ptr %t14, ptr %t15)
  call void @string_builder_append_string(ptr %t14)
  %t16 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t16, i32 %t4)
  call void @string_builder_append_string(ptr %t16)
  %t17 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t17, i64 %t13)
  %t18 = load { ptr, i64 }, ptr %t17
  call void @$prn({ ptr, i64 } %t18)
  %t19 = call i64 @string_builder_mark()
  %t20 = alloca { ptr, i64 }
  %t21 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 7 }, ptr %t21
  call void @to_string$string(ptr %t20, ptr %t21)
  call void @string_builder_append_string(ptr %t20)
  %t22 = alloca { ptr, i64 }
  %t23 = alloca { ptr, i64 }
  store { ptr, i64 } %t6, ptr %t23
  call void @to_string$string(ptr %t22, ptr %t23)
  call void @string_builder_append_string(ptr %t22)
  %t24 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t24, i64 %t19)
  %t25 = load { ptr, i64 }, ptr %t24
  call void @$prn({ ptr, i64 } %t25)
  %t26 = call i64 @string_builder_mark()
  %t27 = alloca { ptr, i64 }
  %t28 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t28
  call void @to_string$string(ptr %t27, ptr %t28)
  call void @string_builder_append_string(ptr %t27)
  %t29 = alloca { ptr, i64 }
  call void @to_string$bool(ptr %t29, i1 %t8)
  call void @string_builder_append_string(ptr %t29)
  %t30 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t30, i64 %t26)
  %t31 = load { ptr, i64 }, ptr %t30
  call void @$prn({ ptr, i64 } %t31)
  ret i32 %t4
}

@$main = alias i32 (), ptr @fn.0
