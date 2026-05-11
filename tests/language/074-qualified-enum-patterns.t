Maybe :: enum { None Some(i32) Pair(i32, i32) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        Maybe.None => 0
        Maybe.Some(as x) => x
        Maybe.Pair(as left, as right) => left + right
    }
}

main :: fn () -> i32 {
    return score(Maybe.Pair(10, 20))
}
¬
30
¬

¬
hir 0
bind Maybe = type.0
bind score = fn.0
bind main = fn.1
type type.0 = Maybe
func fn.0(value: Maybe) -> i32 {
  return i32 on Maybe local.0(value) {
    value(Maybe field(Maybe bind.0(Maybe), None)) => {
      expr i32 0
    }
    enum_variant(Maybe Maybe.Some, as x) => {
      expr i32 local.1(x)
    }
    enum_variant(Maybe Maybe.Pair, as left, as right) => {
      expr i32 add(i32 local.2(left), i32 local.3(right))
    }
  }
}
func fn.1() -> i32 {
  return i32 call bind.1(score)(Maybe call Maybe field(Maybe bind.0(Maybe), Pair)(i32 10, i32 20))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0({ i64, i64 } %value) {
  %t0 = insertvalue { i64, i64 } poison, i64 0, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = extractvalue { i64, i64 } %value, 0
  %t3 = extractvalue { i64, i64 } %t1, 0
  %t4 = icmp eq i64 %t2, %t3
  br i1 %t4, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  %t5 = extractvalue { i64, i64 } %value, 0
  %t6 = icmp eq i64 %t5, 1
  %t7 = extractvalue { i64, i64 } %value, 1
  %t8 = trunc i64 %t7 to i32
  %t9 = and i1 %t6, 1
  br i1 %t9, label %on.body.4, label %on.next.5
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.next.5:
  %t10 = extractvalue { i64, i64 } %value, 0
  %t11 = icmp eq i64 %t10, 2
  %t12 = extractvalue { i64, i64 } %value, 1
  %t14 = alloca i64
  store i64 %t12, ptr %t14
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
  unreachable
on.end.0:
  %t20 = phi i32 [0, %on.value.3], [%t8, %on.value.6], [%t19, %on.value.9]
  ret i32 %t20
}

define i32 @fn.1() {
  %t0 = insertvalue { i32, i32 } poison, i32 10, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 20, 1
  %t3 = alloca i64
  store i64 0, ptr %t3
  store { i32, i32 } %t1, ptr %t3
  %t2 = load i64, ptr %t3
  %t4 = insertvalue { i64, i64 } poison, i64 2, 0
  %t5 = insertvalue { i64, i64 } %t4, i64 %t2, 1
  %t6 = call i32 @fn.0({ i64, i64 } %t5)
  ret i32 %t6
}

@$score = internal alias i32 ({ i64, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
