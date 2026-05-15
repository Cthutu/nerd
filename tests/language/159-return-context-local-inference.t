LRESULT :: isize

callback :: fn () -> LRESULT {
    result := 0
    return result
}

main :: fn () -> i32 {
    return callback().as(i32)
}
¬
0
¬

¬
hir 0
bind LRESULT = type.0
bind callback = fn.0
bind main = fn.1
type type.0 = isize
func fn.0() -> isize {
  let result: isize = isize 0
  return isize local.0(result)
}
func fn.1() -> i32 {
  return i32 cast(isize call bind.1(callback)() as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i64 @fn.0() {
  ret i64 0
}

define internal i32 @fn.1() {
  %t0 = call i64 @fn.0()
  %t1 = trunc i64 %t0 to i32
  ret i32 %t1
}

@$main = alias i32 (), ptr @fn.1
¬
