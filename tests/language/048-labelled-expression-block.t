use std.io

main :: fn () {
    answer :: $answer {
        break $answer 41 + 1
    }
    word :: $word {
        break $word "labelled"
    }
    outer :: $outer {
        $inner {
            break $outer 99
        }
        break $outer 0
    }
    hits := 0
    $void {
        hits += 1
        break $void
    }

    prn($"answer = {answer}")
    prn($"word = {word}")
    prn($"outer = {outer}")
    prn($"hits = {hits}")
    return outer
}
¬
99
¬
answer = 42
word = labelled
outer = 99
hits = 1

¬
hir 0
module module.0(048-labelled-expression-block.input)
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
  let answer: untyped integer = untyped integer block $answer {
    break $answer untyped integer add(untyped integer 41, untyped integer 1)
  }
  let word: string = string block $word {
    break $word string "labelled"
  }
  let outer: untyped integer = untyped integer block $outer {
    expr void block $inner {
    break $outer untyped integer 99
  }
    break $outer untyped integer 0
  }
  let hits: i32 = untyped integer 0
  expr void block $void {
    assign i32 local.3(hits) = i32 add(i32 local.3(hits), i32 1)
    break $void
  }
  expr void call bind.2(prn)(string interpolate(<unknown> "answer = ", untyped integer local.0(answer)))
  expr void call bind.2(prn)(string interpolate(<unknown> "word = ", string local.1(word)))
  expr void call bind.2(prn)(string interpolate(<unknown> "outer = ", untyped integer local.2(outer)))
  expr void call bind.2(prn)(string interpolate(<unknown> "hits = ", i32 local.3(hits)))
  return untyped integer local.2(outer)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"labelled\00"
@.str.m0.1 = private unnamed_addr constant [10 x i8] c"answer = \00"
@.str.m0.2 = private unnamed_addr constant [8 x i8] c"word = \00"
@.str.m0.3 = private unnamed_addr constant [9 x i8] c"outer = \00"
@.str.m0.4 = private unnamed_addr constant [8 x i8] c"hits = \00"

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
  %t1 = add i32 41, 1
  store i32 %t1, ptr %t0, align 4
  br label %block.end.0
block.end.0:
  %t2 = load i32, ptr %t0, align 4
  %t3 = alloca { ptr, i64 }, align 4
  store { ptr, i64 } zeroinitializer, ptr %t3, align 4
  store { ptr, i64 } { ptr @.str.m0.0, i64 8 }, ptr %t3, align 4
  br label %block.end.1
block.end.1:
  %t4 = load { ptr, i64 }, ptr %t3, align 4
  %t5 = alloca i32, align 4
  store i32 0, ptr %t5, align 4
  store i32 99, ptr %t5, align 4
  br label %block.end.2
block.end.3:
  store i32 0, ptr %t5, align 4
  br label %block.end.2
block.end.2:
  %t6 = load i32, ptr %t5, align 4
  %local.3 = alloca i32
  store i32 0, ptr %local.3
  %t7 = load i32, ptr %local.3
  %t8 = add i32 %t7, 1
  store i32 %t8, ptr %local.3
  br label %block.end.4
block.end.4:
  %t9 = call i64 @string_builder_mark()
  %t10 = alloca { ptr, i64 }
  %t11 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 9 }, ptr %t11
  call void @to_string$string(ptr %t10, ptr %t11)
  call void @string_builder_append_string(ptr %t10)
  %t12 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t12, i32 %t2)
  call void @string_builder_append_string(ptr %t12)
  %t13 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t13, i64 %t9)
  %t14 = load { ptr, i64 }, ptr %t13
  call void @$prn({ ptr, i64 } %t14)
  %t15 = call i64 @string_builder_mark()
  %t16 = alloca { ptr, i64 }
  %t17 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 7 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = alloca { ptr, i64 }
  %t19 = alloca { ptr, i64 }
  store { ptr, i64 } %t4, ptr %t19
  call void @to_string$string(ptr %t18, ptr %t19)
  call void @string_builder_append_string(ptr %t18)
  %t20 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t20, i64 %t15)
  %t21 = load { ptr, i64 }, ptr %t20
  call void @$prn({ ptr, i64 } %t21)
  %t22 = call i64 @string_builder_mark()
  %t23 = alloca { ptr, i64 }
  %t24 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 8 }, ptr %t24
  call void @to_string$string(ptr %t23, ptr %t24)
  call void @string_builder_append_string(ptr %t23)
  %t25 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t25, i32 %t6)
  call void @string_builder_append_string(ptr %t25)
  %t26 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t26, i64 %t22)
  %t27 = load { ptr, i64 }, ptr %t26
  call void @$prn({ ptr, i64 } %t27)
  %t28 = call i64 @string_builder_mark()
  %t29 = alloca { ptr, i64 }
  %t30 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 7 }, ptr %t30
  call void @to_string$string(ptr %t29, ptr %t30)
  call void @string_builder_append_string(ptr %t29)
  %t31 = load i32, ptr %local.3
  %t32 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t32, i32 %t31)
  call void @string_builder_append_string(ptr %t32)
  %t33 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t33, i64 %t28)
  %t34 = load { ptr, i64 }, ptr %t33
  call void @$prn({ ptr, i64 } %t34)
  ret i32 %t6
}

@$main = alias i32 (), ptr @fn.0
