-- Chooses between two values with a short-form boolean `on` expression.
enabled: bool = yes

main :: fn () => on enabled => 42 else 7
¬
42
¬

¬
hir 0
bind enabled = value.0
bind main = fn.0
global value.0: bool = bool yes
func fn.0() -> i32 {
  return untyped integer on bool bind.0(enabled) {
    value(bool yes) => {
      expr untyped integer 42
    }
    else => {
      expr untyped integer 7
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@$enabled = internal global i1 0

define void @m0.init() {
  store i1 1, ptr @$enabled
  ret void
}

define i32 @fn.0() {
  %t0 = load i1, ptr @$enabled
  %t1 = icmp eq i1 %t0, 1
  br i1 %t1, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t2 = phi i32 [42, %on.value.3], [7, %on.value.6]
  ret i32 %t2
}

@$main = alias i32 (), ptr @fn.0
