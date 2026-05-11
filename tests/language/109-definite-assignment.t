choose_with_else :: fn () -> i32 {
    value: i32 = undefined
    on yes => value = 21 else value = 3
    return value
}

choose_bool :: fn (flag: bool) -> i32 {
    value: i32 = undefined
    on flag {
        yes => value = 2
        else => value = 4
    }
    return value
}

main :: fn () -> i32 {
    return choose_with_else() + choose_bool(yes) + choose_bool(no)
}
¬
27
¬

¬
hir 0
bind choose_with_else = fn.0
bind choose_bool = fn.1
bind main = fn.2
func fn.0() -> i32 {
  expr <unknown> default
  let value: i32 = <unknown> default
  expr void on bool yes {
    value(bool yes) => {
      assign i32 local.0(value) = i32 21
    }
    else => {
      assign i32 local.0(value) = i32 3
    }
  }
  return i32 local.0(value)
}
func fn.1(flag: bool) -> i32 {
  expr <unknown> default
  let value: i32 = <unknown> default
  expr void on bool local.1(flag) {
    value(bool yes) => {
      assign i32 local.2(value) = i32 2
    }
    else => {
      assign i32 local.2(value) = i32 4
    }
  }
  return i32 local.2(value)
}
func fn.2() -> i32 {
  return i32 add(i32 add(i32 call bind.0(choose_with_else)(), i32 call bind.1(choose_bool)(bool yes)), i32 call bind.1(choose_bool)(bool no))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  %t0 = icmp eq i1 1, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  store i32 21, ptr %local.0
  br label %on.end.0
on.next.2:
  br label %on.body.3
on.body.3:
  store i32 3, ptr %local.0
  br label %on.end.0
on.end.0:
  %t1 = load i32, ptr %local.0
  ret i32 %t1
}

define internal i32 @fn.1(i1 %flag) {
  %local.2 = alloca i32
  store i32 0, ptr %local.2
  %t0 = icmp eq i1 %flag, 1
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  store i32 2, ptr %local.2
  br label %on.end.0
on.next.2:
  br label %on.body.3
on.body.3:
  store i32 4, ptr %local.2
  br label %on.end.0
on.end.0:
  %t1 = load i32, ptr %local.2
  ret i32 %t1
}

define internal i32 @fn.2() {
  %t0 = call i32 @fn.0()
  %t1 = call i32 @fn.1(i1 1)
  %t2 = add i32 %t0, %t1
  %t3 = call i32 @fn.1(i1 0)
  %t4 = add i32 %t2, %t3
  ret i32 %t4
}

@$choose_with_else = internal alias i32 (), ptr @fn.0
@$choose_bool = internal alias i32 (i1), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
