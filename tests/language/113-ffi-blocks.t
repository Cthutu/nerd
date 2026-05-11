-- Declares multiple foreign functions in one FFI block.
ffi "c" {
    absolute :: abs (i32) -> i32
    pub c_strlen :: strlen (^u8) -> usize
}

main :: fn () -> i32 {
    return absolute(-7) + c_strlen(c"nerd").as(i32)
}
¬
11
¬

¬
hir 0
module module.0(113-ffi-blocks.input)
bind absolute = fn.0
bind c_strlen = fn.1
bind main = fn.2
extern func fn.0(i32) -> i32
extern func fn.1(^u8) -> usize
func fn.2() -> i32 {
  return i32 add(i32 call bind.0(absolute)(i32 negate(i32 7)), i32 cast(usize call bind.1(c_strlen)(^u8 c"nerd") as i32))
}
export bind.1(c_strlen)
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"nerd\00"

declare i32 @abs(i32)

declare i64 @strlen(ptr)

define i32 @fn.2() {
  %t0 = sub i32 0, 7
  %t1 = call i32 @abs(i32 %t0)
  %t2 = call i64 @strlen(ptr @.str.m0.1)
  %t3 = trunc i64 %t2 to i32
  %t4 = add i32 %t1, %t3
  ret i32 %t4
}

define i64 @$c_strlen(ptr %p0) {
  %t0 = call i64 @strlen(ptr %p0)
  ret i64 %t0
}
@$main = alias i32 (), ptr @fn.2
