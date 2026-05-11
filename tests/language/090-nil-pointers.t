take_slice :: fn (bytes: []u8) -> i32 {
    on bytes == nil => return 2
    return 0
}

ret_ptr :: fn () -> ^i32 {
    return nil
}

main :: fn () -> i32 {
    value: i32 = 7
    ptr: ^i32 = nil
    bytes: []u8 = nil

    ptr = ^value
    ptr^ = 9
    ptr = nil

    on take_slice(nil) != 3 - 1 => return 3
    on take_slice(bytes) != 3 - 1 => return 4
    ptr = ret_ptr()

    return value - 9
}
¬
0
¬

¬
hir 0
bind take_slice = fn.0
bind ret_ptr = fn.1
bind main = fn.2
func fn.0(bytes: []u8) -> i32 {
  expr void on bool equal([]u8 local.0(bytes), []u8 nil) {
    value(bool yes) => {
      return i32 2
    }
  }
  return i32 0
}
func fn.1() -> ^i32 {
  return ^i32 nil
}
func fn.2() -> i32 {
  let value: i32 = i32 7
  let ptr: ^i32 = ^i32 nil
  let bytes: []u8 = []u8 nil
  assign ^i32 local.2(ptr) = ^i32 address_of(i32 local.1(value))
  assign i32 deref(^i32 local.2(ptr)) = i32 9
  assign ^i32 local.2(ptr) = ^i32 nil
  expr void on bool not_equal(i32 call bind.0(take_slice)([]u8 nil), i32 subtract(i32 3, i32 1)) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool not_equal(i32 call bind.0(take_slice)([]u8 local.3(bytes)), i32 subtract(i32 3, i32 1)) {
    value(bool yes) => {
      return i32 4
    }
  }
  assign ^i32 local.2(ptr) = ^i32 call bind.1(ret_ptr)()
  return i32 subtract(i32 local.1(value), i32 9)
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0({ ptr, i64 } %bytes) {
  %t1 = extractvalue { ptr, i64 } %bytes, 0
  %t2 = extractvalue { ptr, i64 } %bytes, 1
  %t3 = extractvalue { ptr, i64 } zeroinitializer, 0
  %t4 = extractvalue { ptr, i64 } zeroinitializer, 1
  %t5 = icmp eq ptr %t1, %t3
  %t6 = icmp eq i64 %t2, %t4
  %t7 = and i1 %t5, %t6
  %t8 = icmp eq i1 %t7, 1
  br i1 %t8, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 2
on.end.0:
  ret i32 0
}

define ptr @fn.1() {
  ret ptr null
}

define i32 @fn.2() {
  %local.2 = alloca ptr
  store ptr null, ptr %local.2
  %local.1 = alloca i32
  store i32 7, ptr %local.1
  store ptr %local.1, ptr %local.2
  %t0 = load ptr, ptr %local.2
  store i32 9, ptr %t0
  store ptr null, ptr %local.2
  %t1 = call i32 @fn.0({ ptr, i64 } zeroinitializer)
  %t2 = sub i32 3, 1
  %t3 = icmp ne i32 %t1, %t2
  %t4 = icmp eq i1 %t3, 1
  br i1 %t4, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 3
on.end.0:
  %t5 = call i32 @fn.0({ ptr, i64 } zeroinitializer)
  %t6 = sub i32 3, 1
  %t7 = icmp ne i32 %t5, %t6
  %t8 = icmp eq i1 %t7, 1
  br i1 %t8, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 4
on.end.2:
  %t9 = call ptr @fn.1()
  store ptr %t9, ptr %local.2
  %t10 = load i32, ptr %local.1
  %t11 = sub i32 %t10, 9
  ret i32 %t11
}

@$take_slice = internal alias i32 ({ ptr, i64 }), ptr @fn.0
@$ret_ptr = internal alias ptr (), ptr @fn.1
@$main = alias i32 (), ptr @fn.2
