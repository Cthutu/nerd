accept_void :: fn (ptr: ^void) -> i32 {
    view := ptr.as([]u8, 1)
    return view[0].as(i32) - 7
}

main :: fn () -> i32 {
    value: u8 = 7
    ptr := ^value
    return accept_void(ptr)
}
¬
0
¬

¬
hir 0
bind accept_void = fn.0
bind main = fn.1
func fn.0(ptr: ^void) -> i32 {
  let view: []u8 = []u8 cast(^void local.0(ptr) as []u8, usize 1)
  return i32 subtract(i32 cast(u8 index([]u8 local.1(view), untyped integer 0) as i32), i32 7)
}
func fn.1() -> i32 {
  let value: u8 = u8 7
  let ptr: ^u8 = ^u8 address_of(u8 local.2(value))
  return i32 call bind.0(accept_void)(^u8 local.3(ptr))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0(ptr %ptr) {
  %t0 = insertvalue { ptr, i64 } poison, ptr %ptr, 0
  %t1 = insertvalue { ptr, i64 } %t0, i64 1, 1
  %t2 = extractvalue { ptr, i64 } %t1, 0
  %t3 = getelementptr inbounds i8, ptr %t2, i32 0
  %t4 = load i8, ptr %t3
  %t5 = zext i8 %t4 to i32
  %t6 = sub i32 %t5, 7
  ret i32 %t6
}

define i32 @fn.1() {
  %local.2 = alloca i8
  store i8 7, ptr %local.2
  %t0 = call i32 @fn.0(ptr %local.2)
  ret i32 %t0
}

@$accept_void = alias i32 (ptr), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
