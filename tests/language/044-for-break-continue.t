main :: fn () {
    total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => continue
        on i == 5 => break
        total += i
    }
    return total
}
¬
8
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let total: i32 = untyped integer 0
  expr void for c_style {
    init {
      let i: i32 = untyped integer 0
    }
    condition bool less(i32 local.1(i), i32 6)
    body {
      expr void on bool equal(i32 local.1(i), i32 2) {
    value(bool yes) => {
      continue
    }
  }
      expr void on bool equal(i32 local.1(i), i32 5) {
    value(bool yes) => {
      break
    }
  }
      assign i32 local.0(total) = i32 add(i32 local.0(total), i32 local.1(i))
    }
    update {
      assign i32 local.1(i) = i32 add(i32 local.1(i), i32 1)
    }
  }
  return i32 local.0(total)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  %local.1 = alloca i32
  store i32 0, ptr %local.1
  br label %for.cond.0
for.cond.0:
  %t0 = load i32, ptr %local.1
  %t1 = icmp slt i32 %t0, 6
  br i1 %t1, label %for.body.1, label %for.end.3
for.body.1:
  %t2 = load i32, ptr %local.1
  %t3 = icmp eq i32 %t2, 2
  %t4 = icmp eq i1 %t3, 1
  br i1 %t4, label %on.body.5, label %on.end.4
on.body.5:
  br label %for.update.2
on.end.4:
  %t5 = load i32, ptr %local.1
  %t6 = icmp eq i32 %t5, 5
  %t7 = icmp eq i1 %t6, 1
  br i1 %t7, label %on.body.7, label %on.end.6
on.body.7:
  br label %for.end.3
on.end.6:
  %t8 = load i32, ptr %local.0
  %t9 = load i32, ptr %local.1
  %t10 = add i32 %t8, %t9
  store i32 %t10, ptr %local.0
  br label %for.update.2
for.update.2:
  %t11 = load i32, ptr %local.1
  %t12 = add i32 %t11, 1
  store i32 %t12, ptr %local.1
  br label %for.cond.0
for.end.3:
  %t13 = load i32, ptr %local.0
  ret i32 %t13
}

@$main = alias i32 (), ptr @fn.0
