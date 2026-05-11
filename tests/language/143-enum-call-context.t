Code :: enum {
    None
    Home
    End
}

score :: fn (code: Code) -> i32 {
    on code == Home => return 10
    on code == End => return 20
    return 0
}

main :: fn () -> i32 {
    home: Code = Home
    return score(home) + score(Code.End)
}
¬
30
¬

¬
hir 0
bind Code = type.0
bind score = fn.0
bind main = fn.1
type type.0 = Code
func fn.0(code: Code) -> i32 {
  expr void on bool equal(Code local.0(code), Code Home) {
    value(bool yes) => {
      return i32 10
    }
  }
  expr void on bool equal(Code local.0(code), Code End) {
    value(bool yes) => {
      return i32 20
    }
  }
  return i32 0
}
func fn.1() -> i32 {
  let home: Code = Code Home
  return i32 add(i32 call bind.1(score)(Code local.1(home)), i32 call bind.1(score)(Code field(Code bind.0(Code), End)))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0({ i64, i64 } %code) {
  %t0 = insertvalue { i64, i64 } poison, i64 1, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t3 = extractvalue { i64, i64 } %code, 0
  %t4 = extractvalue { i64, i64 } %t1, 0
  %t2 = icmp eq i64 %t3, %t4
  %t5 = icmp eq i1 %t2, 1
  br i1 %t5, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 10
on.end.0:
  %t6 = insertvalue { i64, i64 } poison, i64 2, 0
  %t7 = insertvalue { i64, i64 } %t6, i64 0, 1
  %t9 = extractvalue { i64, i64 } %code, 0
  %t10 = extractvalue { i64, i64 } %t7, 0
  %t8 = icmp eq i64 %t9, %t10
  %t11 = icmp eq i1 %t8, 1
  br i1 %t11, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 20
on.end.2:
  ret i32 0
}

define i32 @fn.1() {
  %t0 = insertvalue { i64, i64 } poison, i64 1, 0
  %t1 = insertvalue { i64, i64 } %t0, i64 0, 1
  %t2 = call i32 @fn.0({ i64, i64 } %t1)
  %t3 = insertvalue { i64, i64 } poison, i64 2, 0
  %t4 = insertvalue { i64, i64 } %t3, i64 0, 1
  %t5 = call i32 @fn.0({ i64, i64 } %t4)
  %t6 = add i32 %t2, %t5
  ret i32 %t6
}

@$score = internal alias i32 ({ i64, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
