use std.io

Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Some(x) => x
        Pair(left, right) => left + right
        Text(_) => 100
    }
}

main :: fn () -> i32 {
    a : Maybe = None
    b : Maybe = Some(5)
    c := Maybe.Pair(10, 20)
    d : Maybe = Text("hello")

    prn($"scores {score(a)} {score(b)} {score(c)} {score(d)}")

    return score(c)
}
¬
30
¬
scores 0 5 30 100

¬
hir 0
module module.0(065-enum-payloads.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind Maybe = type.0
bind score = fn.0
bind main = fn.1
type type.0 = Maybe
func fn.0(value: Maybe) -> i32 {
  return i32 on Maybe local.0(value) {
    value(Maybe None) => {
      expr i32 0
    }
    enum_variant(Some, as x) => {
      expr i32 local.1(x)
    }
    enum_variant(Pair, as left, as right) => {
      expr i32 add(i32 local.2(left), i32 local.3(right))
    }
    enum_variant(Text, _) => {
      expr i32 100
    }
  }
}
func fn.1() -> i32 {
  let a: Maybe = Maybe None
  let b: Maybe = Maybe call Some(i32 5)
  let c: Maybe = Maybe call Maybe field(Maybe bind.2(Maybe), Pair)(i32 10, i32 20)
  let d: Maybe = Maybe call Text(string "hello")
  expr void call bind.0(prn)(string interpolate(<unknown> "scores ", i32 call bind.3(score)(Maybe local.4(a)), <unknown> " ", i32 call bind.3(score)(Maybe local.5(b)), <unknown> " ", i32 call bind.3(score)(Maybe local.6(c)), <unknown> " ", i32 call bind.3(score)(Maybe local.7(d))))
  return i32 call bind.3(score)(Maybe local.6(c))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [35 x i8] c"tests/language/065-enum-payloads.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"scores \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"

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

define internal i32 @fn.0({ i64, i128 } %value) {
  %t14 = alloca i128
  %t24 = alloca i128
  %t0 = insertvalue { i64, i128 } poison, i64 0, 0
  %t1 = insertvalue { i64, i128 } %t0, i128 0, 1
  %t2 = extractvalue { i64, i128 } %value, 0
  %t3 = extractvalue { i64, i128 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = extractvalue { i64, i128 } %value, 0
  %t6 = icmp eq i64 %t5, 1
  %t7 = extractvalue { i64, i128 } %value, 1
  %t8 = trunc i128 %t7 to i32
  %t9 = and i1 %t6, 1
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = extractvalue { i64, i128 } %value, 0
  %t11 = icmp eq i64 %t10, 2
  %t12 = extractvalue { i64, i128 } %value, 1
  store i128 %t12, ptr %t14
  %t13 = load { i32, i32 }, ptr %t14
  %t15 = extractvalue { i32, i32 } %t13, 0
  %t16 = and i1 %t11, 1
  %t17 = extractvalue { i32, i32 } %t13, 1
  %t18 = and i1 %t16, 1
  br i1 %t18, label %on.body.7, label %on.next.8
on.body.7:
  %t19 = add i32 %t15, %t17
  br label %on.value.9
on.value.9:
  br label %on.end.0
on.next.8:
  %t20 = extractvalue { i64, i128 } %value, 0
  %t21 = icmp eq i64 %t20, 3
  %t22 = extractvalue { i64, i128 } %value, 1
  store i128 %t22, ptr %t24
  %t23 = load { ptr, i64 }, ptr %t24
  %t25 = and i1 %t21, 1
  br i1 %t25, label %on.body.10, label %on.next.11
on.body.10:
  br label %on.value.12
on.value.12:
  br label %on.end.0
on.next.11:
  unreachable
on.end.0:
  %t26 = phi i32 [0, %on.value.3], [%t8, %on.value.6], [%t19, %on.value.9], [100, %on.value.12]
  ret i32 %t26
}

define internal i32 @fn.1() {
  %t8 = alloca i128
  %t12 = alloca i128
  %t17 = alloca { ptr, i64 }
  %t21 = alloca { ptr, i64 }
  %t25 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  %t0 = insertvalue { i64, i128 } poison, i64 0, 0
  %t1 = insertvalue { i64, i128 } %t0, i128 0, 1
  %t2 = zext i32 5 to i128
  %t3 = insertvalue { i64, i128 } poison, i64 1, 0
  %t4 = insertvalue { i64, i128 } %t3, i128 %t2, 1
  %t5 = insertvalue { i32, i32 } poison, i32 10, 0
  %t6 = insertvalue { i32, i32 } %t5, i32 20, 1
  store i128 0, ptr %t8
  store { i32, i32 } %t6, ptr %t8
  %t7 = load i128, ptr %t8
  %t9 = insertvalue { i64, i128 } poison, i64 2, 0
  %t10 = insertvalue { i64, i128 } %t9, i128 %t7, 1
  store i128 0, ptr %t12
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t12
  %t11 = load i128, ptr %t12
  %t13 = insertvalue { i64, i128 } poison, i64 3, 0
  %t14 = insertvalue { i64, i128 } %t13, i128 %t11, 1
  %t15 = call i64 @string_builder_mark()
  %t16 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 7 }, ptr %t17
  call void @to_string$string(ptr %t16, ptr %t17)
  call void @string_builder_append_string(ptr %t16)
  %t18 = call i32 @fn.0({ i64, i128 } %t1)
  %t19 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t19, i32 %t18)
  call void @string_builder_append_string(ptr %t19)
  %t20 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.2, i64 1 }, ptr %t21
  call void @to_string$string(ptr %t20, ptr %t21)
  call void @string_builder_append_string(ptr %t20)
  %t22 = call i32 @fn.0({ i64, i128 } %t4)
  %t23 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t23, i32 %t22)
  call void @string_builder_append_string(ptr %t23)
  %t24 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.3, i64 1 }, ptr %t25
  call void @to_string$string(ptr %t24, ptr %t25)
  call void @string_builder_append_string(ptr %t24)
  %t26 = call i32 @fn.0({ i64, i128 } %t10)
  %t27 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t27, i32 %t26)
  call void @string_builder_append_string(ptr %t27)
  %t28 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.4, i64 1 }, ptr %t29
  call void @to_string$string(ptr %t28, ptr %t29)
  call void @string_builder_append_string(ptr %t28)
  %t30 = call i32 @fn.0({ i64, i128 } %t14)
  %t31 = alloca { ptr, i64 }
  call void @to_string$i32(ptr %t31, i32 %t30)
  call void @string_builder_append_string(ptr %t31)
  %t32 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t32, i64 %t15)
  %t33 = load { ptr, i64 }, ptr %t32
  call void @$prn({ ptr, i64 } %t33)
  %t34 = call i32 @fn.0({ i64, i128 } %t10)
  ret i32 %t34
}

@$score = internal alias i32 ({ i64, i128 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
