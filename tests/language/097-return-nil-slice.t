make :: fn (ok: bool) -> []u8 {
    on ok == no => return nil

    text: [2]u8 = ['o', 'k']
    return text[..]
}

main :: fn () -> i32 {
    empty := make(no)
    full := make(yes)

    on empty != nil => return 1
    on full == nil => return 2
    return 0
}
¬
0
¬

¬
hir 0
bind make = fn.0
bind main = fn.1
func fn.0(ok: bool) -> []u8 {
  expr void on bool equal(bool local.0(ok), bool no) {
    value(bool yes) => {
      return []u8 nil
    }
  }
  let text: [2]u8 = [2]u8 array(u8 111, u8 107)
  return []u8 slice([2]u8 local.1(text), <none>, <none>)
}
func fn.1() -> i32 {
  let empty: []u8 = []u8 call bind.0(make)(bool no)
  let full: []u8 = []u8 call bind.0(make)(bool yes)
  expr void on bool not_equal([]u8 local.2(empty), []u8 nil) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool equal([]u8 local.3(full), []u8 nil) {
    value(bool yes) => {
      return i32 2
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal { ptr, i64 } @fn.0(i1 %ok) {
  %t0 = icmp eq i1 %ok, 0
  %t1 = icmp eq i1 %t0, 1
  br i1 %t1, label %on.body.1, label %on.end.0
on.body.1:
  ret { ptr, i64 } zeroinitializer
on.end.0:
  %t2 = insertvalue [2 x i8] poison, i8 111, 0
  %t3 = insertvalue [2 x i8] %t2, i8 107, 1
  %local.1 = alloca [2 x i8]
  store [2 x i8] %t3, ptr %local.1
  %t4 = getelementptr inbounds [2 x i8], ptr %local.1, i64 0, i64 0
  %t5 = insertvalue { ptr, i64 } poison, ptr %t4, 0
  %t6 = insertvalue { ptr, i64 } %t5, i64 2, 1
  ret { ptr, i64 } %t6
}

define internal i32 @fn.1() {
  %t0 = call { ptr, i64 } @fn.0(i1 0)
  %t1 = call { ptr, i64 } @fn.0(i1 1)
  %t3 = extractvalue { ptr, i64 } %t0, 0
  %t4 = extractvalue { ptr, i64 } %t0, 1
  %t5 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t6 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t7 = icmp eq ptr %t3, %t5
  %t8 = icmp eq i64 %t4, %t6
  %t9 = and i1 %t7, %t8
  %t2 = xor i1 %t9, 1
  %t10 = icmp eq i1 %t2, 1
  br i1 %t10, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 1
on.end.0:
  %t12 = extractvalue { ptr, i64 } %t1, 0
  %t13 = extractvalue { ptr, i64 } %t1, 1
  %t14 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t15 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t16 = icmp eq ptr %t12, %t14
  %t17 = icmp eq i64 %t13, %t15
  %t18 = and i1 %t16, %t17
  %t19 = icmp eq i1 %t18, 1
  br i1 %t19, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 2
on.end.2:
  ret i32 0
}

@$make = internal alias { ptr, i64 } (i1), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
