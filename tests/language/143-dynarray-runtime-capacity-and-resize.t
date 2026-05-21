make :: fn (initial_size: usize) -> i32 {
    values: [initial_size..]i32
    on values.capacity < initial_size => return 1
    on values.count != 0 => return 2

    values.resize_undefined_to(initial_size)
    values[0] = 7
    values[1] = 8
    values[2] = 9

    values.resize_to(5)
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
  expr <unknown> default
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
  expr void call fn (usize) -> void field([..]i32 local.1(values), resize_undefined_to)(usize local.0(initial_size))
  assign i32 index([..]i32 local.1(values), untyped integer 0) = i32 7
  assign i32 index([..]i32 local.1(values), untyped integer 1) = i32 8
  assign i32 index([..]i32 local.1(values), untyped integer 2) = i32 9
  expr void call fn (usize) -> void field([..]i32 local.1(values), resize_to)(usize 5)
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

@.macro.file.m0 = private unnamed_addr constant [80 x i8] c"tests/language/143-dynarray-runtime-capacity-and-resize.t\00"

declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

define internal i32 @fn.0(i64 %initial_size) {
  %t1 = mul i64 %initial_size, 4
  %t2 = add i64 24, %t1
  %t0 = call ptr @nrt_mem_alloc(i64 %t2, i64 16, ptr @.macro.file.m0, i32 0)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 0
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 1
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 2
  %t6 = getelementptr inbounds i8, ptr %t0, i64 24
  store ptr %t6, ptr %t3
  store i64 0, ptr %t4
  store i64 %initial_size, ptr %t5
  %local.1 = alloca ptr
  store ptr %t6, ptr %local.1
  %t7 = load ptr, ptr %local.1
  %t8 = alloca i64
  %t9 = icmp eq ptr %t7, null
  br i1 %t9, label %dynarray.field.empty.0, label %dynarray.field.load.1
dynarray.field.empty.0:
  store i64 0, ptr %t8
  br label %dynarray.field.done.2
dynarray.field.load.1:
  %t10 = getelementptr inbounds i8, ptr %t7, i64 -24
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t10, i64 0, i32 2
  %t12 = load i64, ptr %t11
  store i64 %t12, ptr %t8
  br label %dynarray.field.done.2
dynarray.field.done.2:
  %t13 = load i64, ptr %t8
  %t14 = icmp ult i64 %t13, %initial_size
  %t15 = icmp eq i1 %t14, 1
  br i1 %t15, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t16 = load ptr, ptr %local.1
  %t17 = alloca i64
  %t18 = icmp eq ptr %t16, null
  br i1 %t18, label %dynarray.field.empty.5, label %dynarray.field.load.6
dynarray.field.empty.5:
  store i64 0, ptr %t17
  br label %dynarray.field.done.7
dynarray.field.load.6:
  %t19 = getelementptr inbounds i8, ptr %t16, i64 -24
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 1
  %t21 = load i64, ptr %t20
  store i64 %t21, ptr %t17
  br label %dynarray.field.done.7
dynarray.field.done.7:
  %t22 = load i64, ptr %t17
  %t23 = icmp ne i64 %t22, 0
  %t24 = icmp eq i1 %t23, 1
  br i1 %t24, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 2
on.end.8:
  %t25 = load ptr, ptr %local.1
  %t26 = icmp eq ptr %t25, null
  br i1 %t26, label %dynarray.alloc.10, label %dynarray.ready.11
dynarray.alloc.10:
  %t27 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 6)
  %t28 = getelementptr inbounds { ptr, i64, i64 }, ptr %t27, i64 0, i32 0
  %t29 = getelementptr inbounds { ptr, i64, i64 }, ptr %t27, i64 0, i32 1
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t27, i64 0, i32 2
  %t31 = getelementptr inbounds i8, ptr %t27, i64 24
  store ptr %t31, ptr %t28
  store i64 0, ptr %t29
  store i64 0, ptr %t30
  store ptr %t31, ptr %local.1
  br label %dynarray.ready.11
dynarray.ready.11:
  %t32 = load ptr, ptr %local.1
  %t33 = getelementptr inbounds i8, ptr %t32, i64 -24
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 0
  %t35 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 1
  %t36 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 2
  %t37 = load ptr, ptr %t34
  %t38 = load i64, ptr %t35
  %t39 = load i64, ptr %t36
  %t40 = icmp ugt i64 %initial_size, %t39
  br i1 %t40, label %dynarray.resize.grow.12, label %dynarray.resize.count.14
dynarray.resize.grow.12:
  %t41 = mul i64 %initial_size, 4
  %t42 = add i64 24, %t41
  %t43 = call ptr @nrt_mem_realloc(ptr %t33, i64 %t42, i64 16, ptr @.macro.file.m0, i32 6)
  %t44 = getelementptr inbounds i8, ptr %t43, i64 24
  %t45 = getelementptr inbounds { ptr, i64, i64 }, ptr %t43, i64 0, i32 0
  %t46 = getelementptr inbounds { ptr, i64, i64 }, ptr %t43, i64 0, i32 2
  store ptr %t44, ptr %t45
  store i64 %initial_size, ptr %t46
  store ptr %t44, ptr %local.1
  br label %dynarray.resize.count.14
dynarray.resize.count.14:
  %t47 = load ptr, ptr %local.1
  %t48 = getelementptr inbounds i8, ptr %t47, i64 -24
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t48, i64 0, i32 1
  store i64 %initial_size, ptr %t49
  %t50 = load ptr, ptr %local.1
  %t51 = getelementptr inbounds i8, ptr %t50, i64 -24
  %t52 = getelementptr inbounds { ptr, i64, i64 }, ptr %t51, i64 0, i32 0
  %t53 = load ptr, ptr %t52
  %t54 = getelementptr inbounds i32, ptr %t53, i32 0
  store i32 7, ptr %t54
  %t55 = load ptr, ptr %local.1
  %t56 = getelementptr inbounds i8, ptr %t55, i64 -24
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 0
  %t58 = load ptr, ptr %t57
  %t59 = getelementptr inbounds i32, ptr %t58, i32 1
  store i32 8, ptr %t59
  %t60 = load ptr, ptr %local.1
  %t61 = getelementptr inbounds i8, ptr %t60, i64 -24
  %t62 = getelementptr inbounds { ptr, i64, i64 }, ptr %t61, i64 0, i32 0
  %t63 = load ptr, ptr %t62
  %t64 = getelementptr inbounds i32, ptr %t63, i32 2
  store i32 9, ptr %t64
  %t65 = load ptr, ptr %local.1
  %t66 = icmp eq ptr %t65, null
  br i1 %t66, label %dynarray.alloc.15, label %dynarray.ready.16
dynarray.alloc.15:
  %t67 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 11)
  %t68 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 0
  %t69 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 1
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 2
  %t71 = getelementptr inbounds i8, ptr %t67, i64 24
  store ptr %t71, ptr %t68
  store i64 0, ptr %t69
  store i64 0, ptr %t70
  store ptr %t71, ptr %local.1
  br label %dynarray.ready.16
dynarray.ready.16:
  %t72 = load ptr, ptr %local.1
  %t73 = getelementptr inbounds i8, ptr %t72, i64 -24
  %t74 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 0
  %t75 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 1
  %t76 = getelementptr inbounds { ptr, i64, i64 }, ptr %t73, i64 0, i32 2
  %t77 = load ptr, ptr %t74
  %t78 = load i64, ptr %t75
  %t79 = load i64, ptr %t76
  %t80 = icmp ugt i64 5, %t79
  br i1 %t80, label %dynarray.resize.grow.17, label %dynarray.resize.init.18
dynarray.resize.grow.17:
  %t81 = mul i64 5, 4
  %t82 = add i64 24, %t81
  %t83 = call ptr @nrt_mem_realloc(ptr %t73, i64 %t82, i64 16, ptr @.macro.file.m0, i32 11)
  %t84 = getelementptr inbounds i8, ptr %t83, i64 24
  %t85 = getelementptr inbounds { ptr, i64, i64 }, ptr %t83, i64 0, i32 0
  %t86 = getelementptr inbounds { ptr, i64, i64 }, ptr %t83, i64 0, i32 2
  store ptr %t84, ptr %t85
  store i64 5, ptr %t86
  store ptr %t84, ptr %local.1
  br label %dynarray.resize.init.18
dynarray.resize.init.18:
  %t87 = load ptr, ptr %local.1
  %t88 = getelementptr inbounds i8, ptr %t87, i64 -24
  %t89 = getelementptr inbounds { ptr, i64, i64 }, ptr %t88, i64 0, i32 0
  %t90 = icmp ugt i64 5, %t78
  br i1 %t90, label %dynarray.resize.init.loop.20, label %dynarray.resize.count.19
dynarray.resize.init.loop.20:
  %t91 = alloca i64
  store i64 %t78, ptr %t91
  br label %dynarray.resize.init.body.21
dynarray.resize.init.body.21:
  %t92 = load i64, ptr %t91
  %t93 = icmp ult i64 %t92, 5
  br i1 %t93, label %dynarray.resize.init.store.22, label %dynarray.resize.count.19
dynarray.resize.init.store.22:
  %t94 = load ptr, ptr %t89
  %t95 = getelementptr inbounds i32, ptr %t94, i64 %t92
  store i32 0, ptr %t95
  %t96 = add i64 %t92, 1
  store i64 %t96, ptr %t91
  br label %dynarray.resize.init.body.21
dynarray.resize.count.19:
  %t97 = load ptr, ptr %local.1
  %t98 = getelementptr inbounds i8, ptr %t97, i64 -24
  %t99 = getelementptr inbounds { ptr, i64, i64 }, ptr %t98, i64 0, i32 1
  store i64 5, ptr %t99
  %t100 = load ptr, ptr %local.1
  %t101 = alloca i64
  %t102 = icmp eq ptr %t100, null
  br i1 %t102, label %dynarray.field.empty.23, label %dynarray.field.load.24
dynarray.field.empty.23:
  store i64 0, ptr %t101
  br label %dynarray.field.done.25
dynarray.field.load.24:
  %t103 = getelementptr inbounds i8, ptr %t100, i64 -24
  %t104 = getelementptr inbounds { ptr, i64, i64 }, ptr %t103, i64 0, i32 1
  %t105 = load i64, ptr %t104
  store i64 %t105, ptr %t101
  br label %dynarray.field.done.25
dynarray.field.done.25:
  %t106 = load i64, ptr %t101
  %t107 = icmp ne i64 %t106, 5
  %t108 = icmp eq i1 %t107, 1
  br i1 %t108, label %on.body.27, label %on.end.26
on.body.27:
  ret i32 3
on.end.26:
  %t109 = load ptr, ptr %local.1
  %t110 = getelementptr inbounds i8, ptr %t109, i64 -24
  %t111 = getelementptr inbounds { ptr, i64, i64 }, ptr %t110, i64 0, i32 0
  %t112 = load ptr, ptr %t111
  %t113 = getelementptr inbounds i32, ptr %t112, i32 3
  %t114 = load i32, ptr %t113
  %t115 = icmp ne i32 %t114, 0
  %t116 = icmp eq i1 %t115, 1
  br i1 %t116, label %on.body.29, label %on.end.28
on.body.29:
  ret i32 4
on.end.28:
  %t117 = load ptr, ptr %local.1
  %t118 = getelementptr inbounds i8, ptr %t117, i64 -24
  %t119 = getelementptr inbounds { ptr, i64, i64 }, ptr %t118, i64 0, i32 0
  %t120 = load ptr, ptr %t119
  %t121 = getelementptr inbounds i32, ptr %t120, i32 4
  %t122 = load i32, ptr %t121
  %t123 = icmp ne i32 %t122, 0
  %t124 = icmp eq i1 %t123, 1
  br i1 %t124, label %on.body.31, label %on.end.30
on.body.31:
  ret i32 5
on.end.30:
  %t125 = load ptr, ptr %local.1
  %t126 = getelementptr inbounds i8, ptr %t125, i64 -24
  %t127 = getelementptr inbounds { ptr, i64, i64 }, ptr %t126, i64 0, i32 0
  %t128 = load ptr, ptr %t127
  %t129 = getelementptr inbounds i32, ptr %t128, i32 0
  %t130 = load i32, ptr %t129
  %t131 = load ptr, ptr %local.1
  %t132 = getelementptr inbounds i8, ptr %t131, i64 -24
  %t133 = getelementptr inbounds { ptr, i64, i64 }, ptr %t132, i64 0, i32 0
  %t134 = load ptr, ptr %t133
  %t135 = getelementptr inbounds i32, ptr %t134, i32 1
  %t136 = load i32, ptr %t135
  %t137 = add i32 %t130, %t136
  %t138 = load ptr, ptr %local.1
  %t139 = getelementptr inbounds i8, ptr %t138, i64 -24
  %t140 = getelementptr inbounds { ptr, i64, i64 }, ptr %t139, i64 0, i32 0
  %t141 = load ptr, ptr %t140
  %t142 = getelementptr inbounds i32, ptr %t141, i32 2
  %t143 = load i32, ptr %t142
  %t144 = add i32 %t137, %t143
  %t145 = load ptr, ptr %local.1
  %t146 = icmp eq ptr %t145, null
  br i1 %t146, label %dynarray.free.done.33, label %dynarray.free.32
dynarray.free.32:
  %t147 = getelementptr inbounds i8, ptr %t145, i64 -24
  call void @nrt_mem_free(ptr %t147)
  store ptr null, ptr %local.1
  br label %dynarray.free.done.33
dynarray.free.done.33:
  ret i32 %t144
}

define internal i32 @fn.1() {
  %t0 = call i32 @fn.0(i64 3)
  ret i32 %t0
}

@$make = internal alias i32 (i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
