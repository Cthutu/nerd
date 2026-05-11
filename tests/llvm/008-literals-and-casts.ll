to_i32 :: fn(value: f64) -> i32 {
    return value.as(i32)
}

main :: fn() -> i32 {
    number: f64 = 1.5
    _text: string = "hello"
    _c_text: ^u8 = c"hello"
    _ptr: ^i32 = nil
    return to_i32(number)
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"hello\00"

define i32 @fn.0(double %value) {
  %t0 = fptosi double %value to i32
  ret i32 %t0
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0(double 0x3FF8000000000000)
  ret i32 %t0
}

@$to_i32 = internal alias i32 (double), ptr @fn.0
@$main = alias i32 (), ptr @fn.1

