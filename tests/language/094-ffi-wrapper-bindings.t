c_realloc :: ffi "c" realloc (^void, usize) -> ^void

realloc :: fn (ptr: ^void, size: usize) -> ^void {
    return c_realloc(ptr, size)
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
bind c_realloc = fn.0
bind realloc = fn.1
bind main = fn.2
extern func fn.0(^void, usize) -> ^void
func fn.1(ptr: ^void, size: usize) -> ^void {
  return ^void call bind.0(c_realloc)(^void local.0(ptr), usize local.1(size))
}
func fn.2() -> i32 {
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"

declare ptr @realloc(ptr, i64)

define internal ptr @fn.1(ptr %ptr, i64 %size) {
  %t0 = call ptr @realloc(ptr %ptr, i64 %size)
  ret ptr %t0
}

define internal i32 @fn.2() {
  ret i32 0
}

@$realloc = internal alias ptr (ptr, i64), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
