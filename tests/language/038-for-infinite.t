main :: fn () {
    for {
        return 7
    }
}
¬
7
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  expr void for condition {
    body {
      return untyped integer 7
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  br label %for.cond.0
for.cond.0:
  br label %for.body.1
for.body.1:
  ret i32 7
}

@$main = alias i32 (), ptr @fn.0
