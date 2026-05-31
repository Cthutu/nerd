Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

find :: fn (system: ^System, handle: u64) -> ^Info {
    return for info in system.infos {
        break on info.handle == handle => info
    } else {
        break nil
    }
}

main :: fn () -> i32 {
    labelled := $pick {
        break $pick on 1 == 1 => 5
        break $pick 9
    }
    on labelled != 5 => return 1

    infos: [2..]Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := System { infos }
    found  := find(^system, 9)
    on found == nil => return 2
    on found.handle != 9 => return 3

    missing := find(^system, 3)
    on missing != nil => return 4

    return 0
}
¬
0
¬

¬
hir 0
bind Info = type.0
bind System = type.1
bind find = fn.0
bind main = fn.1
type type.0 = Info
type type.1 = System
func fn.0(system: ^System, handle: u64) -> ^Info {
  return ^Info for in info: ^Info in [..]Info field(^System local.0(system), infos) {
    body {
      expr void on bool equal(u64 field(^Info local.2(info), handle), u64 local.1(handle)) {
    value(bool yes) => {
      break ^Info local.2(info)
    }
  }
    }
    else {
      break ^Info nil
    }
  }
}
func fn.1() -> i32 {
  let labelled: i32 = untyped integer block $pick {
    expr void on bool equal(untyped integer 1, untyped integer 1) {
    value(bool yes) => {
      break $pick untyped integer 5
    }
  }
    break $pick untyped integer 9
  }
  expr void on bool not_equal(i32 local.3(labelled), i32 5) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr <unknown> default
  let infos: [..]Info = [..]Info array(; min_capacity 2)
  expr void call fn (Info) -> void field([..]Info local.4(infos), push)(Info plex(handle: u64 7))
  expr void call fn (Info) -> void field([..]Info local.4(infos), push)(Info plex(handle: u64 9))
  let system: System = System plex(infos: [..]Info local.4(infos))
  let found: ^Info = ^Info call bind.2(find)(^System address_of(System local.5(system)), u64 9)
  expr void on bool equal(^Info local.6(found), ^Info nil) {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool not_equal(u64 field(^Info local.6(found), handle), u64 9) {
    value(bool yes) => {
      return i32 3
    }
  }
  let missing: ^Info = ^Info call bind.2(find)(^System address_of(System local.5(system)), u64 3)
  expr void on bool not_equal(^Info local.7(missing), ^Info nil) {
    value(bool yes) => {
      return i32 4
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [30 x i8] c"tests/language/153-break-on.t\00"

declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

define internal ptr @fn.0(ptr %system, i64 %handle) {
  %local.2 = alloca ptr
  %t0 = load { ptr }, ptr %system
  %t1 = extractvalue { ptr } %t0, 0
  %t4 = alloca ptr
  %t5 = alloca i64
  %t6 = icmp eq ptr %t1, null
  br i1 %t6, label %for.in.empty.0, label %for.in.load.1
for.in.empty.0:
  store ptr null, ptr %t4
  store i64 0, ptr %t5
  br label %for.in.ready.2
for.in.load.1:
  %t7 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 0
  %t9 = load ptr, ptr %t8
  %t10 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t10, i64 0, i32 1
  %t12 = load i64, ptr %t11
  store ptr %t9, ptr %t4
  store i64 %t12, ptr %t5
  br label %for.in.ready.2
for.in.ready.2:
  %t2 = load ptr, ptr %t4
  %t3 = load i64, ptr %t5
  %t13 = alloca ptr, align 4
  store ptr null, ptr %t13, align 4
  %t14 = alloca i64
  store i64 0, ptr %t14
  br label %for.in.cond.3
for.in.cond.3:
  %t15 = load i64, ptr %t14
  %t16 = icmp ult i64 %t15, %t3
  br i1 %t16, label %for.in.body.4, label %for.in.else.6
for.in.body.4:
  %t17 = getelementptr inbounds { i64 }, ptr %t2, i64 %t15
  store ptr %t17, ptr %local.2
  %t18 = load ptr, ptr %local.2
  %t19 = load { i64 }, ptr %t18
  %t20 = extractvalue { i64 } %t19, 0
  %t21 = icmp eq i64 %t20, %handle
  %t22 = icmp eq i1 %t21, 1
  br i1 %t22, label %on.body.9, label %on.end.8
on.body.9:
  %t23 = load ptr, ptr %local.2
  store ptr %t23, ptr %t13, align 4
  br label %for.in.end.7
on.end.8:
  br label %for.in.update.5
for.in.update.5:
  %t24 = load i64, ptr %t14
  %t25 = add i64 %t24, 1
  store i64 %t25, ptr %t14
  br label %for.in.cond.3
for.in.else.6:
  store ptr null, ptr %t13, align 4
  br label %for.in.end.7
for.in.end.7:
  %t26 = load ptr, ptr %t13, align 4
  ret ptr %t26
}

define internal i32 @fn.1() {
  %local.4 = alloca ptr
  %local.5 = alloca { ptr }
  %t0 = alloca i32, align 4
  store i32 0, ptr %t0, align 4
  %t1 = icmp eq i32 1, 1
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.2, label %on.end.1
on.body.2:
  store i32 5, ptr %t0, align 4
  br label %block.end.0
on.end.1:
  store i32 9, ptr %t0, align 4
  br label %block.end.0
block.end.0:
  %t3 = load i32, ptr %t0, align 4
  %t4 = icmp ne i32 %t3, 5
  %t5 = icmp eq i1 %t4, 1
  br i1 %t5, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t6 = call ptr @nrt_mem_alloc(i64 40, i64 16, ptr @.macro.file.m0, i32 0)
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 0
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  %t10 = getelementptr inbounds i8, ptr %t6, i64 24
  store ptr %t10, ptr %t7
  store i64 0, ptr %t8
  store i64 2, ptr %t9
  store ptr %t10, ptr %local.4
  %t11 = load ptr, ptr %local.4
  %t12 = icmp eq ptr %t11, null
  br i1 %t12, label %dynarray.alloc.5, label %dynarray.ready.6
dynarray.alloc.5:
  %t13 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 25)
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 0
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 1
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 2
  %t17 = getelementptr inbounds i8, ptr %t13, i64 24
  store ptr %t17, ptr %t14
  store i64 0, ptr %t15
  store i64 0, ptr %t16
  store ptr %t17, ptr %local.4
  br label %dynarray.ready.6
dynarray.ready.6:
  %t18 = load ptr, ptr %local.4
  %t19 = getelementptr inbounds i8, ptr %t18, i64 -24
  %t20 = insertvalue { i64 } poison, i64 7, 0
  %t21 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 0
  %t22 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 1
  %t23 = getelementptr inbounds { ptr, i64, i64 }, ptr %t19, i64 0, i32 2
  %t24 = load ptr, ptr %t21
  %t25 = load i64, ptr %t22
  %t26 = load i64, ptr %t23
  %t27 = add i64 %t25, 1
  %t28 = icmp ugt i64 %t27, %t26
  br i1 %t28, label %dynarray.grow.7, label %dynarray.store.8
dynarray.grow.7:
  %t29 = icmp eq i64 %t26, 0
  %t30 = mul i64 %t26, 2
  %t31 = select i1 %t29, i64 1, i64 %t30
  %t32 = mul i64 %t31, 8
  %t33 = add i64 24, %t32
  %t34 = call ptr @nrt_mem_realloc(ptr %t19, i64 %t33, i64 16, ptr @.macro.file.m0, i32 25)
  %t35 = getelementptr inbounds i8, ptr %t34, i64 24
  %t36 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 0
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 2
  store ptr %t35, ptr %t36
  store i64 %t31, ptr %t37
  store ptr %t35, ptr %local.4
  br label %dynarray.store.8
dynarray.store.8:
  %t38 = load ptr, ptr %local.4
  %t39 = getelementptr inbounds i8, ptr %t38, i64 -24
  %t40 = getelementptr inbounds { ptr, i64, i64 }, ptr %t39, i64 0, i32 0
  %t41 = getelementptr inbounds { ptr, i64, i64 }, ptr %t39, i64 0, i32 1
  %t42 = load ptr, ptr %t40
  %t43 = getelementptr inbounds { i64 }, ptr %t42, i64 %t25
  store { i64 } %t20, ptr %t43
  store i64 %t27, ptr %t41
  %t44 = load ptr, ptr %local.4
  %t45 = icmp eq ptr %t44, null
  br i1 %t45, label %dynarray.alloc.9, label %dynarray.ready.10
dynarray.alloc.9:
  %t46 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 26)
  %t47 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 0
  %t48 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 1
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 2
  %t50 = getelementptr inbounds i8, ptr %t46, i64 24
  store ptr %t50, ptr %t47
  store i64 0, ptr %t48
  store i64 0, ptr %t49
  store ptr %t50, ptr %local.4
  br label %dynarray.ready.10
dynarray.ready.10:
  %t51 = load ptr, ptr %local.4
  %t52 = getelementptr inbounds i8, ptr %t51, i64 -24
  %t53 = insertvalue { i64 } poison, i64 9, 0
  %t54 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 0
  %t55 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 1
  %t56 = getelementptr inbounds { ptr, i64, i64 }, ptr %t52, i64 0, i32 2
  %t57 = load ptr, ptr %t54
  %t58 = load i64, ptr %t55
  %t59 = load i64, ptr %t56
  %t60 = add i64 %t58, 1
  %t61 = icmp ugt i64 %t60, %t59
  br i1 %t61, label %dynarray.grow.11, label %dynarray.store.12
dynarray.grow.11:
  %t62 = icmp eq i64 %t59, 0
  %t63 = mul i64 %t59, 2
  %t64 = select i1 %t62, i64 1, i64 %t63
  %t65 = mul i64 %t64, 8
  %t66 = add i64 24, %t65
  %t67 = call ptr @nrt_mem_realloc(ptr %t52, i64 %t66, i64 16, ptr @.macro.file.m0, i32 26)
  %t68 = getelementptr inbounds i8, ptr %t67, i64 24
  %t69 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 0
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t67, i64 0, i32 2
  store ptr %t68, ptr %t69
  store i64 %t64, ptr %t70
  store ptr %t68, ptr %local.4
  br label %dynarray.store.12
dynarray.store.12:
  %t71 = load ptr, ptr %local.4
  %t72 = getelementptr inbounds i8, ptr %t71, i64 -24
  %t73 = getelementptr inbounds { ptr, i64, i64 }, ptr %t72, i64 0, i32 0
  %t74 = getelementptr inbounds { ptr, i64, i64 }, ptr %t72, i64 0, i32 1
  %t75 = load ptr, ptr %t73
  %t76 = getelementptr inbounds { i64 }, ptr %t75, i64 %t58
  store { i64 } %t53, ptr %t76
  store i64 %t60, ptr %t74
  %t77 = load ptr, ptr %local.4
  %t78 = insertvalue { ptr } poison, ptr %t77, 0
  store { ptr } %t78, ptr %local.5
  %t79 = call ptr @fn.0(ptr %local.5, i64 9)
  %t80 = icmp eq ptr %t79, null
  %t81 = icmp eq i1 %t80, 1
  br i1 %t81, label %on.body.14, label %on.end.13
on.body.14:
  ret i32 2
on.end.13:
  %t82 = load { i64 }, ptr %t79
  %t83 = extractvalue { i64 } %t82, 0
  %t84 = icmp ne i64 %t83, 9
  %t85 = icmp eq i1 %t84, 1
  br i1 %t85, label %on.body.16, label %on.end.15
on.body.16:
  ret i32 3
on.end.15:
  %t86 = call ptr @fn.0(ptr %local.5, i64 3)
  %t87 = icmp ne ptr %t86, null
  %t88 = icmp eq i1 %t87, 1
  br i1 %t88, label %on.body.18, label %on.end.17
on.body.18:
  ret i32 4
on.end.17:
  ret i32 0
}

@$find = internal alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
