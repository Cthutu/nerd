main :: fn () -> i32 {
    item: i32
    ptr: ^i32 = ^item
    nil_ptr: ^i32
    void_ptr: ^void = ptr

    on nil_ptr {
        nil => {}
        else => return 1
    }

    on ptr == nil => return 2
    on nil != nil_ptr => return 3
    on void_ptr != ptr => return 4
    return 0
}
¬
0
¬

¬
hir 0
bind main = fn.0
func fn.0() -> i32 {
  expr <unknown> default
  let item: i32 = <unknown> default
  let ptr: ^i32 = ^i32 address_of(i32 local.0(item))
  expr <unknown> default
  let nil_ptr: ^i32 = <unknown> default
  let void_ptr: ^void = ^i32 local.1(ptr)
  expr void on ^i32 local.2(nil_ptr) {
    value(^i32 nil) => {
      expr void block {
  }
    }
    else => {
      return i32 1
    }
  }
  expr void on bool equal(^i32 local.1(ptr), ^i32 nil) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool not_equal(^i32 nil, ^i32 local.2(nil_ptr)) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool not_equal(^void local.3(void_ptr), ^i32 local.1(ptr)) {
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
  %local.0 = alloca i32
  store i32 0, ptr %local.0
  %t0 = icmp eq ptr null, null
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %block.end.3
block.end.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  ret i32 1
on.end.0:
  %t1 = icmp eq ptr %local.0, null
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.6, label %on.end.5
on.body.6:
  ret i32 2
on.end.5:
  %t3 = icmp ne ptr null, null
  %t4 = icmp eq i1 %t3, 1
  br i1 %t4, label %on.body.8, label %on.end.7
on.body.8:
  ret i32 3
on.end.7:
  %t5 = icmp ne ptr %local.0, %local.0
  %t6 = icmp eq i1 %t5, 1
  br i1 %t6, label %on.body.10, label %on.end.9
on.body.10:
  ret i32 4
on.end.9:
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
