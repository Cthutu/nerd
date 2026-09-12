first :: fn (_fixed: i32, args: ...) -> i32 {
    return args.next[i32]()
}
main :: fn () { _value := first(0, 42) }
¬
define internal i32 @fn.0(i32 %_fixed, ...) {
  %t0 = call ptr @nrt_va_create()
  call void @llvm.va_start.p0(ptr %t0)
  %t1 = call i32 @nrt_va_next_i32(ptr %t0)
  call void @nrt_va_done(ptr %t0)
  ret i32 %t1
}
