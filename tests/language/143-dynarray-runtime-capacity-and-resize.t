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
  let values: [..]i32 = [..]i32 array(; min_capacity usize local.0(initial_size))
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
  %t0 = call ptr @malloc(i64 24)
  %t1 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 0
  %t2 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 1
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 2
  %t4 = mul i64 %initial_size, 4
  %t5 = call ptr @malloc(i64 %t4)
  store ptr %t5, ptr %t1
  store i64 0, ptr %t2
  store i64 %initial_size, ptr %t3
  %local.1 = alloca ptr
  store ptr %t0, ptr %local.1
  %t6 = load ptr, ptr %local.1
  %t7 = alloca i64
  %t8 = icmp eq ptr %t6, null
  br i1 %t8, label %dynarray.field.empty.0, label %dynarray.field.load.1
dynarray.field.empty.0:
  store i64 0, ptr %t7
  br label %dynarray.field.done.2
dynarray.field.load.1:
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  %t10 = load i64, ptr %t9
  store i64 %t10, ptr %t7
  br label %dynarray.field.done.2
dynarray.field.done.2:
  %t11 = load i64, ptr %t7
  %t12 = icmp slt i64 %t11, %initial_size
  %t13 = icmp eq i1 %t12, 1
  br i1 %t13, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t14 = load ptr, ptr %local.1
  %t15 = alloca i64
  %t16 = icmp eq ptr %t14, null
  br i1 %t16, label %dynarray.field.empty.5, label %dynarray.field.load.6
dynarray.field.empty.5:
  store i64 0, ptr %t15
  br label %dynarray.field.done.7
dynarray.field.load.6:
  %t17 = getelementptr inbounds { ptr, i64, i64 }, ptr %t14, i64 0, i32 1
  %t18 = load i64, ptr %t17
  store i64 %t18, ptr %t15
  br label %dynarray.field.done.7
dynarray.field.done.7:
  %t19 = load i64, ptr %t15
  %t20 = icmp ne i64 %t19, 0
  %t21 = icmp eq i1 %t20, 1
  br i1 %t21, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 2
on.end.8:
  %t22 = load ptr, ptr %local.1
  %t23 = icmp eq ptr %t22, null
  br i1 %t23, label %dynarray.alloc.10, label %dynarray.ready.11
dynarray.alloc.10:
  %t24 = call ptr @malloc(i64 24)
  %t25 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 0
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 1
  %t27 = getelementptr inbounds { ptr, i64, i64 }, ptr %t24, i64 0, i32 2
  store ptr null, ptr %t25
  store i64 0, ptr %t26
  store i64 0, ptr %t27
  store ptr %t24, ptr %local.1
  br label %dynarray.ready.11
dynarray.ready.11:
  %t28 = load ptr, ptr %local.1
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 0
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 1
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 2
  %t32 = load ptr, ptr %t29
  %t33 = load i64, ptr %t30
  %t34 = load i64, ptr %t31
  %t35 = icmp ugt i64 %initial_size, %t34
  br i1 %t35, label %dynarray.resize.grow.12, label %dynarray.resize.count.14
dynarray.resize.grow.12:
  %t36 = mul i64 %initial_size, 4
  %t37 = call ptr @realloc(ptr %t32, i64 %t36)
  store ptr %t37, ptr %t29
  store i64 %initial_size, ptr %t31
  br label %dynarray.resize.count.14
dynarray.resize.count.14:
  store i64 %initial_size, ptr %t30
  %t38 = load ptr, ptr %local.1
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t38, i64 0, i32 0
  %t40 = load ptr, ptr %t39
  %t41 = getelementptr inbounds i32, ptr %t40, i32 0
  store i32 7, ptr %t41
  %t42 = load ptr, ptr %local.1
  %t43 = getelementptr inbounds { ptr, i64, i64 }, ptr %t42, i64 0, i32 0
  %t44 = load ptr, ptr %t43
  %t45 = getelementptr inbounds i32, ptr %t44, i32 1
  store i32 8, ptr %t45
  %t46 = load ptr, ptr %local.1
  %t47 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 0
  %t48 = load ptr, ptr %t47
  %t49 = getelementptr inbounds i32, ptr %t48, i32 2
  store i32 9, ptr %t49
  %t50 = load ptr, ptr %local.1
  %t51 = icmp eq ptr %t50, null
  br i1 %t51, label %dynarray.alloc.15, label %dynarray.ready.16
dynarray.alloc.15:
  %t52 = call ptr @malloc(i64 24)
  %t53 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 0
  %t54 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 1
  %t55 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 2
  store ptr null, ptr %t53
  store i64 0, ptr %t54
  store i64 0, ptr %t55
  store ptr %t52, ptr %local.1
  br label %dynarray.ready.16
dynarray.ready.16:
  %t56 = load ptr, ptr %local.1
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 0
  %t58 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 1
  %t59 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 2
  %t60 = load ptr, ptr %t57
  %t61 = load i64, ptr %t58
  %t62 = load i64, ptr %t59
  %t63 = icmp ugt i64 5, %t62
  br i1 %t63, label %dynarray.resize.grow.17, label %dynarray.resize.init.18
dynarray.resize.grow.17:
  %t64 = mul i64 5, 4
  %t65 = call ptr @realloc(ptr %t60, i64 %t64)
  store ptr %t65, ptr %t57
  store i64 5, ptr %t59
  br label %dynarray.resize.init.18
dynarray.resize.init.18:
  %t66 = icmp ugt i64 5, %t61
  br i1 %t66, label %dynarray.resize.init.loop.20, label %dynarray.resize.count.19
dynarray.resize.init.loop.20:
  %t67 = alloca i64
  store i64 %t61, ptr %t67
  br label %dynarray.resize.init.body.21
dynarray.resize.init.body.21:
  %t68 = load i64, ptr %t67
  %t69 = icmp ult i64 %t68, 5
  br i1 %t69, label %dynarray.resize.init.store.22, label %dynarray.resize.count.19
dynarray.resize.init.store.22:
  %t70 = load ptr, ptr %t57
  %t71 = getelementptr inbounds i32, ptr %t70, i64 %t68
  store i32 0, ptr %t71
  %t72 = add i64 %t68, 1
  store i64 %t72, ptr %t67
  br label %dynarray.resize.init.body.21
dynarray.resize.count.19:
  store i64 5, ptr %t58
  %t73 = load ptr, ptr %local.1
  %t74 = alloca i64
  %t75 = icmp eq ptr %t73, null
  br i1 %t75, label %dynarray.field.empty.23, label %dynarray.field.load.24
dynarray.field.empty.23:
  store i64 0, ptr %t74
  br label %dynarray.field.done.25
dynarray.field.load.24:
  %t76 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 1
  %t77 = load i64, ptr %t76
  store i64 %t77, ptr %t74
  br label %dynarray.field.done.25
dynarray.field.done.25:
  %t78 = load i64, ptr %t74
  %t79 = icmp ne i64 %t78, 5
  %t80 = icmp eq i1 %t79, 1
  br i1 %t80, label %on.body.27, label %on.end.26
on.body.27:
  ret i32 3
on.end.26:
  %t81 = load ptr, ptr %local.1
  %t82 = getelementptr inbounds { ptr, i64, i64 }, ptr %t81, i64 0, i32 0
  %t83 = load ptr, ptr %t82
  %t84 = getelementptr inbounds i32, ptr %t83, i32 3
  %t85 = load i32, ptr %t84
  %t86 = icmp ne i32 %t85, 0
  %t87 = icmp eq i1 %t86, 1
  br i1 %t87, label %on.body.29, label %on.end.28
on.body.29:
  ret i32 4
on.end.28:
  %t88 = load ptr, ptr %local.1
  %t89 = getelementptr inbounds { ptr, i64, i64 }, ptr %t88, i64 0, i32 0
  %t90 = load ptr, ptr %t89
  %t91 = getelementptr inbounds i32, ptr %t90, i32 4
  %t92 = load i32, ptr %t91
  %t93 = icmp ne i32 %t92, 0
  %t94 = icmp eq i1 %t93, 1
  br i1 %t94, label %on.body.31, label %on.end.30
on.body.31:
  ret i32 5
on.end.30:
  %t95 = load ptr, ptr %local.1
  %t96 = getelementptr inbounds { ptr, i64, i64 }, ptr %t95, i64 0, i32 0
  %t97 = load ptr, ptr %t96
  %t98 = getelementptr inbounds i32, ptr %t97, i32 0
  %t99 = load i32, ptr %t98
  %t100 = load ptr, ptr %local.1
  %t101 = getelementptr inbounds { ptr, i64, i64 }, ptr %t100, i64 0, i32 0
  %t102 = load ptr, ptr %t101
  %t103 = getelementptr inbounds i32, ptr %t102, i32 1
  %t104 = load i32, ptr %t103
  %t105 = add i32 %t99, %t104
  %t106 = load ptr, ptr %local.1
  %t107 = getelementptr inbounds { ptr, i64, i64 }, ptr %t106, i64 0, i32 0
  %t108 = load ptr, ptr %t107
  %t109 = getelementptr inbounds i32, ptr %t108, i32 2
  %t110 = load i32, ptr %t109
  %t111 = add i32 %t105, %t110
  %t112 = load ptr, ptr %local.1
  %t113 = icmp eq ptr %t112, null
  br i1 %t113, label %dynarray.free.done.33, label %dynarray.free.32
dynarray.free.32:
  %t114 = getelementptr inbounds { ptr, i64, i64 }, ptr %t112, i64 0, i32 0
  %t115 = load ptr, ptr %t114
  call void @free(ptr %t115)
  call void @free(ptr %t112)
  store ptr null, ptr %local.1
  br label %dynarray.free.done.33
dynarray.free.done.33:
  ret i32 %t111
}

define i32 @fn.1() {
  %t0 = call i32 @fn.0(i64 3)
  ret i32 %t0
}

@$make = internal alias i32 (i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
