main :: fn () -> i32 {
    empty: []u8 = nil
    text: [2]u8 = ['a', 'b']
    view := text[..]

    on empty != nil => return 1
    on nil != empty => return 2
    on view == nil => return 3
    on nil == view => return 4

    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let empty: []u8 = []u8 nil
  let text: [2]u8 = [2]u8 array(u8 97, u8 98)
  let view: []u8 = []u8 slice([2]u8 local.1(text), <none>, <none>)
  expr void on bool not_equal([]u8 local.0(empty), []u8 nil) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool not_equal([]u8 nil, []u8 local.0(empty)) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool equal([]u8 local.2(view), []u8 nil) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool equal([]u8 nil, []u8 local.2(view)) {
    value(bool yes) => {
      return i32 4
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = insertvalue [2 x i8] poison, i8 97, 0
  %t1 = insertvalue [2 x i8] %t0, i8 98, 1
  %local.1 = alloca [2 x i8]
  store [2 x i8] %t1, ptr %local.1
  %t2 = getelementptr inbounds [2 x i8], ptr %local.1, i64 0, i64 0
  %t3 = insertvalue { ptr, i64 } poison, ptr %t2, 0
  %t4 = insertvalue { ptr, i64 } %t3, i64 2, 1
  %t6 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t7 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t8 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t9 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t10 = icmp eq ptr %t6, %t8
  %t11 = icmp eq i64 %t7, %t9
  %t12 = and i1 %t10, %t11
  %t5 = xor i1 %t12, 1
  %t13 = icmp eq i1 %t5, 1
  br i1 %t13, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 1
on.end.0:
  %t15 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t16 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t17 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t18 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t19 = icmp eq ptr %t15, %t17
  %t20 = icmp eq i64 %t16, %t18
  %t21 = and i1 %t19, %t20
  %t14 = xor i1 %t21, 1
  %t22 = icmp eq i1 %t14, 1
  br i1 %t22, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 2
on.end.2:
  %t24 = extractvalue { ptr, i64 } %t4, 0
  %t25 = extractvalue { ptr, i64 } %t4, 1
  %t26 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t27 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t28 = icmp eq ptr %t24, %t26
  %t29 = icmp eq i64 %t25, %t27
  %t30 = and i1 %t28, %t29
  %t31 = icmp eq i1 %t30, 1
  br i1 %t31, label %on.body.5, label %on.end.4
on.body.5:
  ret i32 3
on.end.4:
  %t33 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t34 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t35 = extractvalue { ptr, i64 } %t4, 0
  %t36 = extractvalue { ptr, i64 } %t4, 1
  %t37 = icmp eq ptr %t33, %t35
  %t38 = icmp eq i64 %t34, %t36
  %t39 = and i1 %t37, %t38
  %t40 = icmp eq i1 %t39, 1
  br i1 %t40, label %on.body.7, label %on.end.6
on.body.7:
  ret i32 4
on.end.6:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
