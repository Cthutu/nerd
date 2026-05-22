Info :: plex {
    handle u64
}

System :: plex {
    infos [..]Info
}

find :: fn (system: ^System, handle: u64) -> ^Info {
    return for info in system.infos {
        on info.handle == handle => break info
    } else {
        break nil
    }
}

main :: fn () -> i32 {
    infos: [2..]Info
    infos.push({ handle: 7 })
    infos.push({ handle: 9 })

    system := System { infos }
    found  := find(^system, 9)
    on found == nil => return 1
    on found.handle != 9 => return 2

    missing := find(^system, 3)
    on missing != nil => return 3

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
  expr <unknown> default
  let infos: [..]Info = [..]Info array(; min_capacity 2)
  expr void call fn (Info) -> void field([..]Info local.3(infos), push)(Info plex(handle: u64 7))
  expr void call fn (Info) -> void field([..]Info local.3(infos), push)(Info plex(handle: u64 9))
  let system: System = System plex(infos: [..]Info local.3(infos))
  let found: ^Info = ^Info call bind.2(find)(^System address_of(System local.4(system)), u64 9)
  expr void on bool equal(^Info local.5(found), ^Info nil) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool not_equal(u64 field(^Info local.5(found), handle), u64 9) {
    value(bool yes) => {
      return i32 2
    }
  }
  let missing: ^Info = ^Info call bind.2(find)(^System address_of(System local.4(system)), u64 3)
  expr void on bool not_equal(^Info local.6(missing), ^Info nil) {
    value(bool yes) => {
      return i32 3
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [45 x i8] c"tests/language/152-on-break-for-expression.t\00"

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
  br i1 %t16, label %for.in.body.4, label %for.in.else.5
for.in.body.4:
  %t17 = getelementptr inbounds { i64 }, ptr %t2, i64 %t15
  store ptr %t17, ptr %local.2
  %t18 = load ptr, ptr %local.2
  %t19 = load { i64 }, ptr %t18
  %t20 = extractvalue { i64 } %t19, 0
  %t21 = icmp eq i64 %t20, %handle
  %t22 = icmp eq i1 %t21, 1
  br i1 %t22, label %on.body.8, label %on.end.7
on.body.8:
  %t23 = load ptr, ptr %local.2
  store ptr %t23, ptr %t13, align 4
  br label %for.in.end.6
on.end.7:
  %t24 = load i64, ptr %t14
  %t25 = add i64 %t24, 1
  store i64 %t25, ptr %t14
  br label %for.in.cond.3
for.in.else.5:
  store ptr null, ptr %t13, align 4
  br label %for.in.end.6
for.in.end.6:
  %t26 = load ptr, ptr %t13, align 4
  ret ptr %t26
}

define internal i32 @fn.1() {
  %local.3 = alloca ptr
  %local.4 = alloca { ptr }
  %t0 = call ptr @nrt_mem_alloc(i64 40, i64 16, ptr @.macro.file.m0, i32 0)
  %t1 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 0
  %t2 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 1
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 2
  %t4 = getelementptr inbounds i8, ptr %t0, i64 24
  store ptr %t4, ptr %t1
  store i64 0, ptr %t2
  store i64 2, ptr %t3
  store ptr %t4, ptr %local.3
  %t5 = load ptr, ptr %local.3
  %t6 = icmp eq ptr %t5, null
  br i1 %t6, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t7 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 19)
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 0
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 1
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 2
  %t11 = getelementptr inbounds i8, ptr %t7, i64 24
  store ptr %t11, ptr %t8
  store i64 0, ptr %t9
  store i64 0, ptr %t10
  store ptr %t11, ptr %local.3
  br label %dynarray.ready.1
dynarray.ready.1:
  %t12 = load ptr, ptr %local.3
  %t13 = getelementptr inbounds i8, ptr %t12, i64 -24
  %t14 = insertvalue { i64 } poison, i64 7, 0
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 0
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 1
  %t17 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 2
  %t18 = load ptr, ptr %t15
  %t19 = load i64, ptr %t16
  %t20 = load i64, ptr %t17
  %t21 = add i64 %t19, 1
  %t22 = icmp ugt i64 %t21, %t20
  br i1 %t22, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t23 = icmp eq i64 %t20, 0
  %t24 = mul i64 %t20, 2
  %t25 = select i1 %t23, i64 1, i64 %t24
  %t26 = mul i64 %t25, 8
  %t27 = add i64 24, %t26
  %t28 = call ptr @nrt_mem_realloc(ptr %t13, i64 %t27, i64 16, ptr @.macro.file.m0, i32 19)
  %t29 = getelementptr inbounds i8, ptr %t28, i64 24
  %t30 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 0
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t28, i64 0, i32 2
  store ptr %t29, ptr %t30
  store i64 %t25, ptr %t31
  store ptr %t29, ptr %local.3
  br label %dynarray.store.3
dynarray.store.3:
  %t32 = load ptr, ptr %local.3
  %t33 = getelementptr inbounds i8, ptr %t32, i64 -24
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 0
  %t35 = getelementptr inbounds { ptr, i64, i64 }, ptr %t33, i64 0, i32 1
  %t36 = load ptr, ptr %t34
  %t37 = getelementptr inbounds { i64 }, ptr %t36, i64 %t19
  store { i64 } %t14, ptr %t37
  store i64 %t21, ptr %t35
  %t38 = load ptr, ptr %local.3
  %t39 = icmp eq ptr %t38, null
  br i1 %t39, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t40 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 20)
  %t41 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 0
  %t42 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 1
  %t43 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 2
  %t44 = getelementptr inbounds i8, ptr %t40, i64 24
  store ptr %t44, ptr %t41
  store i64 0, ptr %t42
  store i64 0, ptr %t43
  store ptr %t44, ptr %local.3
  br label %dynarray.ready.5
dynarray.ready.5:
  %t45 = load ptr, ptr %local.3
  %t46 = getelementptr inbounds i8, ptr %t45, i64 -24
  %t47 = insertvalue { i64 } poison, i64 9, 0
  %t48 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 0
  %t49 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 1
  %t50 = getelementptr inbounds { ptr, i64, i64 }, ptr %t46, i64 0, i32 2
  %t51 = load ptr, ptr %t48
  %t52 = load i64, ptr %t49
  %t53 = load i64, ptr %t50
  %t54 = add i64 %t52, 1
  %t55 = icmp ugt i64 %t54, %t53
  br i1 %t55, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t56 = icmp eq i64 %t53, 0
  %t57 = mul i64 %t53, 2
  %t58 = select i1 %t56, i64 1, i64 %t57
  %t59 = mul i64 %t58, 8
  %t60 = add i64 24, %t59
  %t61 = call ptr @nrt_mem_realloc(ptr %t46, i64 %t60, i64 16, ptr @.macro.file.m0, i32 20)
  %t62 = getelementptr inbounds i8, ptr %t61, i64 24
  %t63 = getelementptr inbounds { ptr, i64, i64 }, ptr %t61, i64 0, i32 0
  %t64 = getelementptr inbounds { ptr, i64, i64 }, ptr %t61, i64 0, i32 2
  store ptr %t62, ptr %t63
  store i64 %t58, ptr %t64
  store ptr %t62, ptr %local.3
  br label %dynarray.store.7
dynarray.store.7:
  %t65 = load ptr, ptr %local.3
  %t66 = getelementptr inbounds i8, ptr %t65, i64 -24
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t66, i64 0, i32 0
  %t68 = getelementptr inbounds { ptr, i64, i64 }, ptr %t66, i64 0, i32 1
  %t69 = load ptr, ptr %t67
  %t70 = getelementptr inbounds { i64 }, ptr %t69, i64 %t52
  store { i64 } %t47, ptr %t70
  store i64 %t54, ptr %t68
  %t71 = load ptr, ptr %local.3
  %t72 = insertvalue { ptr } poison, ptr %t71, 0
  store { ptr } %t72, ptr %local.4
  %t73 = call ptr @fn.0(ptr %local.4, i64 9)
  %t74 = icmp eq ptr %t73, null
  %t75 = icmp eq i1 %t74, 1
  br i1 %t75, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 1
on.end.8:
  %t76 = load { i64 }, ptr %t73
  %t77 = extractvalue { i64 } %t76, 0
  %t78 = icmp ne i64 %t77, 9
  %t79 = icmp eq i1 %t78, 1
  br i1 %t79, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 2
on.end.10:
  %t80 = call ptr @fn.0(ptr %local.4, i64 3)
  %t81 = icmp ne ptr %t80, null
  %t82 = icmp eq i1 %t81, 1
  br i1 %t82, label %on.body.13, label %on.end.12
on.body.13:
  ret i32 3
on.end.12:
  ret i32 0
}

@$find = internal alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
