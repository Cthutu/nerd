use std.io

Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Some(as x) => x
        Pair(as left, as right) => left + right
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
  let c: Maybe = Maybe call Maybe field(Maybe bind.5(Maybe), Pair)(i32 10, i32 20)
  let d: Maybe = Maybe call Text(string "hello")
  expr void call bind.2(prn)(string interpolate(<unknown> "scores ", i32 call bind.6(score)(Maybe local.4(a)), <unknown> " ", i32 call bind.6(score)(Maybe local.5(b)), <unknown> " ", i32 call bind.6(score)(Maybe local.6(c)), <unknown> " ", i32 call bind.6(score)(Maybe local.7(d))))
  return i32 call bind.6(score)(Maybe local.6(c))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.m0.1 = private unnamed_addr constant [8 x i8] c"scores \00"
@.str.m0.2 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.3 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.4 = private unnamed_addr constant [2 x i8] c" \00"

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

define internal i32 @fn.0({ i64, i128 } %value) {
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
  %t14 = alloca i128
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
  %t24 = alloca i128
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
  %t0 = insertvalue { i64, i128 } poison, i64 0, 0
  %t1 = insertvalue { i64, i128 } %t0, i128 0, 1
  %t2 = zext i32 5 to i128
  %t3 = insertvalue { i64, i128 } poison, i64 1, 0
  %t4 = insertvalue { i64, i128 } %t3, i128 %t2, 1
  %t5 = insertvalue { i32, i32 } poison, i32 10, 0
  %t6 = insertvalue { i32, i32 } %t5, i32 20, 1
  %t8 = alloca i128
  store i128 0, ptr %t8
  store { i32, i32 } %t6, ptr %t8
  %t7 = load i128, ptr %t8
  %t9 = insertvalue { i64, i128 } poison, i64 2, 0
  %t10 = insertvalue { i64, i128 } %t9, i128 %t7, 1
  %t12 = alloca i128
  store i128 0, ptr %t12
  store { ptr, i64 } { ptr @.str.m0.0, i64 5 }, ptr %t12
  %t11 = load i128, ptr %t12
  %t13 = insertvalue { i64, i128 } poison, i64 3, 0
  %t14 = insertvalue { i64, i128 } %t13, i128 %t11, 1
  %t15 = call i64 @string_builder_mark()
  %t16 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.1, i64 7 })
  call void @string_builder_append_string({ ptr, i64 } %t16)
  %t17 = call i32 @fn.0({ i64, i128 } %t1)
  %t18 = call { ptr, i64 } @to_string$i32(i32 %t17)
  call void @string_builder_append_string({ ptr, i64 } %t18)
  %t19 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.2, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t19)
  %t20 = call i32 @fn.0({ i64, i128 } %t4)
  %t21 = call { ptr, i64 } @to_string$i32(i32 %t20)
  call void @string_builder_append_string({ ptr, i64 } %t21)
  %t22 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.3, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t22)
  %t23 = call i32 @fn.0({ i64, i128 } %t10)
  %t24 = call { ptr, i64 } @to_string$i32(i32 %t23)
  call void @string_builder_append_string({ ptr, i64 } %t24)
  %t25 = call { ptr, i64 } @to_string$string({ ptr, i64 } { ptr @.str.m0.4, i64 1 })
  call void @string_builder_append_string({ ptr, i64 } %t25)
  %t26 = call i32 @fn.0({ i64, i128 } %t14)
  %t27 = call { ptr, i64 } @to_string$i32(i32 %t26)
  call void @string_builder_append_string({ ptr, i64 } %t27)
  %t28 = call { ptr, i64 } @string_builder_finish(i64 %t15)
  call void @$prn({ ptr, i64 } %t28)
  %t29 = call i32 @fn.0({ i64, i128 } %t10)
  ret i32 %t29
}

@$score = internal alias i32 ({ i64, i128 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
