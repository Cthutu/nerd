main :: fn () -> i32 {
    for {
        value :: 1
        on value {
            1 => break
        }
        return 1
    }
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  expr void for condition {
    body {
      let value: untyped integer = untyped integer 1
      expr void on i32 local.0(value) {
    value(i32 1) => {
      break
    }
  }
      return i32 1
    }
  }
  return <unknown> 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  br label %for.cond.0
for.cond.0:
  br label %for.body.1
for.body.1:
  %t0 = icmp eq i32 1, 1
  br i1 %t0, label %on.body.5, label %on.end.4
on.body.5:
  br label %for.end.3
on.end.4:
  ret i32 1
for.end.3:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
