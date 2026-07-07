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

@.macro.file.m0 = private unnamed_addr constant [39 x i8] c"tests/language/143-enum-call-context.t\00"

define internal i32 @fn.0({ i64, i8 } %code) {
  %t0 = insertvalue { i64, i8 } poison, i64 1, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  %t3 = extractvalue { i64, i8 } %code, 0
  %t4 = extractvalue { i64, i8 } %t1, 0
  %t2 = icmp eq i64 %t3, %t4
  %t5 = icmp eq i1 %t2, 1
  br i1 %t5, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 10
on.end.0:
  %t6 = insertvalue { i64, i8 } poison, i64 2, 0
  %t7 = insertvalue { i64, i8 } %t6, i8 0, 1
  %t9 = extractvalue { i64, i8 } %code, 0
  %t10 = extractvalue { i64, i8 } %t7, 0
  %t8 = icmp eq i64 %t9, %t10
  %t11 = icmp eq i1 %t8, 1
  br i1 %t11, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 20
on.end.2:
  ret i32 0
}

define internal i32 @fn.1() {
  %local.1 = alloca { i64, i8 }
  %t0 = insertvalue { i64, i8 } poison, i64 1, 0
  %t1 = insertvalue { i64, i8 } %t0, i8 0, 1
  store { i64, i8 } %t1, ptr %local.1
  %t2 = load { i64, i8 }, ptr %local.1
  %t3 = call i32 @fn.0({ i64, i8 } %t2)
  %t4 = insertvalue { i64, i8 } poison, i64 2, 0
  %t5 = insertvalue { i64, i8 } %t4, i8 0, 1
  %t6 = call i32 @fn.0({ i64, i8 } %t5)
  %t7 = add i32 %t3, %t6
  ret i32 %t7
}

@$score = internal alias i32 ({ i64, i8 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
