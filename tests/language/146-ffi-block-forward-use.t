wrap :: fn () -> ^void {
    return c_malloc(1)
}

ffi "c" {
    c_malloc :: malloc (num_bytes: usize) -> ^void
    c_free   :: free (ptr: ^void)
}

main :: fn () -> i32 {
    ptr := wrap()
    return on ptr {
        nil => 1
        else => {
            c_free(ptr)
            break 0
        }
    }
}
¬
0
¬

¬
hir 0
bind wrap = fn.0
bind c_malloc = fn.1
bind c_free = fn.2
bind main = fn.3
func fn.0() -> ^void {
  return ^void call decl.1(c_malloc)(usize 1)
}
extern func fn.1(usize) -> ^void
extern func fn.2(^void) -> void
func fn.3() -> i32 {
  let ptr: ^void = ^void call bind.0(wrap)()
  return i32 on ^void local.0(ptr) {
    value(^void nil) => {
      expr i32 1
    }
    else => {
      expr i32 block {
    expr void call bind.2(c_free)(^void local.0(ptr))
    break i32 0
  }
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [2 x i8] c"c\00"

define internal ptr @fn.0() {
  %t0 = call ptr @malloc(i64 1)
  ret ptr %t0
}

declare ptr @malloc(i64)

declare void @free(ptr)

define internal i32 @fn.3() {
  %t0 = call ptr @fn.0()
  %t1 = icmp eq ptr %t0, null
  br i1 %t1, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  %t2 = alloca i32, align 4
  store i32 0, ptr %t2, align 4
  call void @free(ptr %t0)
  store i32 0, ptr %t2, align 4
  br label %block.end.6
block.end.6:
  %t3 = load i32, ptr %t2, align 4
  br label %on.value.7
on.value.7:
  br label %on.end.0
on.end.0:
  %t4 = phi i32 [1, %on.value.3], [%t3, %on.value.7]
  ret i32 %t4
}

@$wrap = internal alias ptr (), ptr @fn.0
@$main = alias i32 (), ptr @fn.3
