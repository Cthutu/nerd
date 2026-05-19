main :: fn () -> i32 {
    null_ptr: ^u8 = 0
    display_addr :: 0x1000
    display: ^void = display_addr
    other := 0x2000.as(^u8)
    resource_addr: usize = 0x3000
    resource := resource_addr.as(^u8)

    on null_ptr != nil => return 1
    on display == nil => return 2
    on other == nil => return 3
    on resource == nil => return 4
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  let null_ptr: ^u8 = ^u8 0
  let display_addr: untyped integer = untyped integer 4096
  let display: ^void = ^void local.0(display_addr)
  let other: ^u8 = ^u8 cast(untyped integer 8192 as ^u8)
  let resource_addr: usize = usize 12288
  let resource: ^u8 = ^u8 cast(usize local.4(resource_addr) as ^u8)
  expr void on bool not_equal(^u8 local.1(null_ptr), ^u8 nil) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool equal(^void local.2(display), ^void nil) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool equal(^u8 local.3(other), ^u8 nil) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool equal(^u8 local.5(resource), ^u8 nil) {
    value(bool yes) => {
      return i32 4
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = inttoptr i32 4096 to ptr
  %t1 = inttoptr i32 8192 to ptr
  %t2 = inttoptr i64 12288 to ptr
  %t3 = icmp ne ptr null, null
  %t4 = icmp eq i1 %t3, 1
  br i1 %t4, label %on.body.1, label %on.end.0
on.body.1:
  ret i32 1
on.end.0:
  %t5 = icmp eq ptr %t0, null
  %t6 = icmp eq i1 %t5, 1
  br i1 %t6, label %on.body.3, label %on.end.2
on.body.3:
  ret i32 2
on.end.2:
  %t7 = icmp eq ptr %t1, null
  %t8 = icmp eq i1 %t7, 1
  br i1 %t8, label %on.body.5, label %on.end.4
on.body.5:
  ret i32 3
on.end.4:
  %t9 = icmp eq ptr %t2, null
  %t10 = icmp eq i1 %t9, 1
  br i1 %t10, label %on.body.7, label %on.end.6
on.body.7:
  ret i32 4
on.end.6:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
