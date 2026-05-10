make :: fn (initial_size: usize) -> i32 {
    values: [initial_size..]i32
    on values.capacity < initial_size => return 1
    on values.count != 0 => return 2

    values.resize_undefined(initial_size)
    values[0] = 7
    values[1] = 8
    values[2] = 9

    values.resize(5)
    on values.count != 5 => return 3
    on values[3] != 0 => return 4
    on values[4] != 0 => return 5

    result := values[0] + values[1] + values[2]
    values.free()
    return result
}

main :: fn () -> i32 {
    return make(3)
}
¬
24
¬

¬
hir 0
bind make = fn.0
bind main = fn.1
func fn.0(initial_size: usize) -> i32 {
  expr <unknown> <unsupported>
  let values: [..]i32 = <unknown> <unsupported>
  expr void on bool less(usize field([..]i32 local.1(values), capacity), usize local.0(initial_size)) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool not_equal(usize field([..]i32 local.1(values), count), usize 0) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void call fn (usize) -> void field([..]i32 local.1(values), resize_undefined)(usize local.0(initial_size))
  assign i32 index([..]i32 local.1(values), untyped integer 0) = i32 7
  assign i32 index([..]i32 local.1(values), untyped integer 1) = i32 8
  assign i32 index([..]i32 local.1(values), untyped integer 2) = i32 9
  expr void call fn (usize) -> void field([..]i32 local.1(values), resize)(usize 5)
  expr void on bool not_equal(usize field([..]i32 local.1(values), count), usize 5) {
    value(bool yes) => {
      return i32 3
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.1(values), untyped integer 3), i32 0) {
    value(bool yes) => {
      return i32 4
    }
  }
  expr void on bool not_equal(i32 index([..]i32 local.1(values), untyped integer 4), i32 0) {
    value(bool yes) => {
      return i32 5
    }
  }
  let result: i32 = i32 add(i32 add(i32 index([..]i32 local.1(values), untyped integer 0), i32 index([..]i32 local.1(values), untyped integer 1)), i32 index([..]i32 local.1(values), untyped integer 2))
  expr void call fn () -> void field([..]i32 local.1(values), free)()
  return i32 local.2(result)
}
func fn.1() -> i32 {
  return i32 call bind.0(make)(usize 3)
}
¬
; nerd llvm-ir 0
; generated from HIR

declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

define i32 @fn.0(i64 %initial_size) {
  %local.1 = alloca ptr
  store ptr null, ptr %local.1
  %t0 = load ptr, ptr %local.1
  %t1 = alloca i64
  %t2 = icmp eq ptr %t0, null
  br i1 %t2, label %dynarray.field.empty.0, label %dynarray.field.load.1
dynarray.field.empty.0:
  store i64 0, ptr %t1
  br label %dynarray.field.done.2
dynarray.field.load.1:
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 2
  %t4 = load i64, ptr %t3
  store i64 %t4, ptr %t1
  br label %dynarray.field.done.2
dynarray.field.done.2:
  %t5 = load i64, ptr %t1
  %t6 = icmp slt i64 %t5, %initial_size
  %t7 = icmp eq i1 %t6, 1
  br i1 %t7, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t8 = load ptr, ptr %local.1
  %t9 = alloca i64
  %t10 = icmp eq ptr %t8, null
  br i1 %t10, label %dynarray.field.empty.5, label %dynarray.field.load.6
dynarray.field.empty.5:
  store i64 0, ptr %t9
  br label %dynarray.field.done.7
dynarray.field.load.6:
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 1
  %t12 = load i64, ptr %t11
  store i64 %t12, ptr %t9
  br label %dynarray.field.done.7
dynarray.field.done.7:
  %t13 = load i64, ptr %t9
  %t14 = icmp ne i64 %t13, 0
  %t15 = icmp eq i1 %t14, 1
  br i1 %t15, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 2
on.end.8:
  %t16 = load ptr, ptr %local.1
  %t17 = icmp eq ptr %t16, null
  br i1 %t17, label %dynarray.alloc.10, label %dynarray.ready.11
dynarray.alloc.10:
  %t18 = call ptr @malloc(i64 24)
  %t19 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 0
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 1
  %t21 = getelementptr inbounds { ptr, i64, i64 }, ptr %t18, i64 0, i32 2
  store ptr null, ptr %t19
  store i64 0, ptr %t20
  store i64 0, ptr %t21
  store ptr %t18, ptr %local.1
  br label %dynarray.ready.11
dynarray.ready.11:
  %t22 = load ptr, ptr %local.1
  %t23 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 0
  %t24 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 1
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 2
  %t26 = load ptr, ptr %t23
  %t27 = load i64, ptr %t24
  %t28 = load i64, ptr %t25
  %t29 = icmp ugt i64 %initial_size, %t28
  br i1 %t29, label %dynarray.resize.grow.12, label %dynarray.resize.count.14
dynarray.resize.grow.12:
  %t30 = mul i64 %initial_size, 4
  %t31 = call ptr @realloc(ptr %t26, i64 %t30)
  store ptr %t31, ptr %t23
  store i64 %initial_size, ptr %t25
  br label %dynarray.resize.count.14
dynarray.resize.count.14:
  store i64 %initial_size, ptr %t24
  %t32 = load ptr, ptr %local.1
  %t33 = getelementptr inbounds { ptr, i64, i64 }, ptr %t32, i64 0, i32 0
  %t34 = load ptr, ptr %t33
  %t35 = getelementptr inbounds i32, ptr %t34, i32 0
  store i32 7, ptr %t35
  %t36 = load ptr, ptr %local.1
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 0
  %t38 = load ptr, ptr %t37
  %t39 = getelementptr inbounds i32, ptr %t38, i32 1
  store i32 8, ptr %t39
  %t40 = load ptr, ptr %local.1
  %t41 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 0
  %t42 = load ptr, ptr %t41
  %t43 = getelementptr inbounds i32, ptr %t42, i32 2
  store i32 9, ptr %t43
  %t44 = load ptr, ptr %local.1
  %t45 = icmp eq ptr %t44, null
  br i1 %t45, label %dynarray.alloc.15, label %dynarray.ready.16
dynarray.alloc.15:
  %t46 = call ptr @malloc(i64 24)
  %t47 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 0
  %t48 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 1
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 2
  store ptr null, ptr %t47
  store i64 0, ptr %t48
  store i64 0, ptr %t49
  store ptr %t46, ptr %local.1
  br label %dynarray.ready.16
dynarray.ready.16:
  %t50 = load ptr, ptr %local.1
  %t51 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 0
  %t52 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 1
  %t53 = getelementptr inbounds { ptr, i64, i64 }, ptr %t50, i64 0, i32 2
  %t54 = load ptr, ptr %t51
  %t55 = load i64, ptr %t52
  %t56 = load i64, ptr %t53
  %t57 = icmp ugt i64 5, %t56
  br i1 %t57, label %dynarray.resize.grow.17, label %dynarray.resize.init.18
dynarray.resize.grow.17:
  %t58 = mul i64 5, 4
  %t59 = call ptr @realloc(ptr %t54, i64 %t58)
  store ptr %t59, ptr %t51
  store i64 5, ptr %t53
  br label %dynarray.resize.init.18
dynarray.resize.init.18:
  %t60 = icmp ugt i64 5, %t55
  br i1 %t60, label %dynarray.resize.init.loop.20, label %dynarray.resize.count.19
dynarray.resize.init.loop.20:
  %t61 = alloca i64
  store i64 %t55, ptr %t61
  br label %dynarray.resize.init.body.21
dynarray.resize.init.body.21:
  %t62 = load i64, ptr %t61
  %t63 = icmp ult i64 %t62, 5
  br i1 %t63, label %dynarray.resize.init.store.22, label %dynarray.resize.count.19
dynarray.resize.init.store.22:
  %t64 = load ptr, ptr %t51
  %t65 = getelementptr inbounds i32, ptr %t64, i64 %t62
  store i32 0, ptr %t65
  %t66 = add i64 %t62, 1
  store i64 %t66, ptr %t61
  br label %dynarray.resize.init.body.21
dynarray.resize.count.19:
  store i64 5, ptr %t52
  %t67 = load ptr, ptr %local.1
  %t68 = alloca i64
  %t69 = icmp eq ptr %t67, null
  br i1 %t69, label %dynarray.field.empty.23, label %dynarray.field.load.24
dynarray.field.empty.23:
  store i64 0, ptr %t68
  br label %dynarray.field.done.25
dynarray.field.load.24:
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 1
  %t71 = load i64, ptr %t70
  store i64 %t71, ptr %t68
  br label %dynarray.field.done.25
dynarray.field.done.25:
  %t72 = load i64, ptr %t68
  %t73 = icmp ne i64 %t72, 5
  %t74 = icmp eq i1 %t73, 1
  br i1 %t74, label %on.body.27, label %on.end.26
on.body.27:
  ret i32 3
on.end.26:
  %t75 = load ptr, ptr %local.1
  %t76 = getelementptr inbounds { ptr, i64, i64 }, ptr %t75, i64 0, i32 0
  %t77 = load ptr, ptr %t76
  %t78 = getelementptr inbounds i32, ptr %t77, i32 3
  %t79 = load i32, ptr %t78
  %t80 = icmp ne i32 %t79, 0
  %t81 = icmp eq i1 %t80, 1
  br i1 %t81, label %on.body.29, label %on.end.28
on.body.29:
  ret i32 4
on.end.28:
  %t82 = load ptr, ptr %local.1
  %t83 = getelementptr inbounds { ptr, i64, i64 }, ptr %t82, i64 0, i32 0
  %t84 = load ptr, ptr %t83
  %t85 = getelementptr inbounds i32, ptr %t84, i32 4
  %t86 = load i32, ptr %t85
  %t87 = icmp ne i32 %t86, 0
  %t88 = icmp eq i1 %t87, 1
  br i1 %t88, label %on.body.31, label %on.end.30
on.body.31:
  ret i32 5
on.end.30:
  %t89 = load ptr, ptr %local.1
  %t90 = getelementptr inbounds { ptr, i64, i64 }, ptr %t89, i64 0, i32 0
  %t91 = load ptr, ptr %t90
  %t92 = getelementptr inbounds i32, ptr %t91, i32 0
  %t93 = load i32, ptr %t92
  %t94 = load ptr, ptr %local.1
  %t95 = getelementptr inbounds { ptr, i64, i64 }, ptr %t94, i64 0, i32 0
  %t96 = load ptr, ptr %t95
  %t97 = getelementptr inbounds i32, ptr %t96, i32 1
  %t98 = load i32, ptr %t97
  %t99 = add i32 %t93, %t98
  %t100 = load ptr, ptr %local.1
  %t101 = getelementptr inbounds { ptr, i64, i64 }, ptr %t100, i64 0, i32 0
  %t102 = load ptr, ptr %t101
  %t103 = getelementptr inbounds i32, ptr %t102, i32 2
  %t104 = load i32, ptr %t103
  %t105 = add i32 %t99, %t104
  %t106 = load ptr, ptr %local.1
  %t107 = icmp eq ptr %t106, null
  br i1 %t107, label %dynarray.free.done.33, label %dynarray.free.32
dynarray.free.32:
  %t108 = getelementptr inbounds { ptr, i64, i64 }, ptr %t106, i64 0, i32 0
  %t109 = load ptr, ptr %t108
  call void @free(ptr %t109)
  call void @free(ptr %t106)
  store ptr null, ptr %local.1
  br label %dynarray.free.done.33
dynarray.free.done.33:
  ret i32 %t105
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0(i64 3)
  ret i32 %t0
}

@$make = alias i32 (i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
