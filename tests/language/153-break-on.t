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

declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

define internal ptr @fn.0(ptr %system, i64 %handle) {
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
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t1, i64 0, i32 0
  %t8 = load ptr, ptr %t7
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t1, i64 0, i32 1
  %t10 = load i64, ptr %t9
  store ptr %t8, ptr %t4
  store i64 %t10, ptr %t5
  br label %for.in.ready.2
for.in.ready.2:
  %t2 = load ptr, ptr %t4
  %t3 = load i64, ptr %t5
  %t11 = alloca ptr, align 4
  store ptr null, ptr %t11, align 4
  %t12 = alloca i64
  store i64 0, ptr %t12
  %local.2 = alloca ptr
  br label %for.in.cond.3
for.in.cond.3:
  %t13 = load i64, ptr %t12
  %t14 = icmp ult i64 %t13, %t3
  br i1 %t14, label %for.in.body.4, label %for.in.else.5
for.in.body.4:
  %t15 = getelementptr inbounds { i64 }, ptr %t2, i64 %t13
  store ptr %t15, ptr %local.2
  %t16 = load ptr, ptr %local.2
  %t17 = load { i64 }, ptr %t16
  %t18 = extractvalue { i64 } %t17, 0
  %t19 = icmp eq i64 %t18, %handle
  %t20 = icmp eq i1 %t19, 1
  br i1 %t20, label %on.body.8, label %on.end.7
on.body.8:
  %t21 = load ptr, ptr %local.2
  store ptr %t21, ptr %t11, align 4
  br label %for.in.end.6
on.end.7:
  %t22 = load i64, ptr %t12
  %t23 = add i64 %t22, 1
  store i64 %t23, ptr %t12
  br label %for.in.cond.3
for.in.else.5:
  store ptr null, ptr %t11, align 4
  br label %for.in.end.6
for.in.end.6:
  %t24 = load ptr, ptr %t11, align 4
  ret ptr %t24
}

define internal i32 @fn.1() {
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
  %t6 = call ptr @malloc(i64 24)
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 0
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  %t10 = call ptr @malloc(i64 16)
  store ptr %t10, ptr %t7
  store i64 0, ptr %t8
  store i64 2, ptr %t9
  %local.4 = alloca ptr
  store ptr %t6, ptr %local.4
  %t11 = load ptr, ptr %local.4
  %t12 = icmp eq ptr %t11, null
  br i1 %t12, label %dynarray.alloc.5, label %dynarray.ready.6
dynarray.alloc.5:
  %t13 = call ptr @malloc(i64 24)
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 0
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 1
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 2
  store ptr null, ptr %t14
  store i64 0, ptr %t15
  store i64 0, ptr %t16
  store ptr %t13, ptr %local.4
  br label %dynarray.ready.6
dynarray.ready.6:
  %t17 = load ptr, ptr %local.4
  %t18 = insertvalue { i64 } poison, i64 7, 0
  %t19 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 0
  %t20 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 1
  %t21 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 2
  %t22 = load ptr, ptr %t19
  %t23 = load i64, ptr %t20
  %t24 = load i64, ptr %t21
  %t25 = add i64 %t23, 1
  %t26 = icmp ugt i64 %t25, %t24
  br i1 %t26, label %dynarray.grow.7, label %dynarray.store.8
dynarray.grow.7:
  %t27 = icmp eq i64 %t24, 0
  %t28 = mul i64 %t24, 2
  %t29 = select i1 %t27, i64 1, i64 %t28
  %t30 = mul i64 %t29, 8
  %t31 = call ptr @realloc(ptr %t22, i64 %t30)
  store ptr %t31, ptr %t19
  store i64 %t29, ptr %t21
  br label %dynarray.store.8
dynarray.store.8:
  %t32 = load ptr, ptr %t19
  %t33 = getelementptr inbounds { i64 }, ptr %t32, i64 %t23
  store { i64 } %t18, ptr %t33
  store i64 %t25, ptr %t20
  %t34 = load ptr, ptr %local.4
  %t35 = icmp eq ptr %t34, null
  br i1 %t35, label %dynarray.alloc.9, label %dynarray.ready.10
dynarray.alloc.9:
  %t36 = call ptr @malloc(i64 24)
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 0
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 1
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t36, i64 0, i32 2
  store ptr null, ptr %t37
  store i64 0, ptr %t38
  store i64 0, ptr %t39
  store ptr %t36, ptr %local.4
  br label %dynarray.ready.10
dynarray.ready.10:
  %t40 = load ptr, ptr %local.4
  %t41 = insertvalue { i64 } poison, i64 9, 0
  %t42 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 0
  %t43 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 1
  %t44 = getelementptr inbounds { ptr, i64, i64 }, ptr %t40, i64 0, i32 2
  %t45 = load ptr, ptr %t42
  %t46 = load i64, ptr %t43
  %t47 = load i64, ptr %t44
  %t48 = add i64 %t46, 1
  %t49 = icmp ugt i64 %t48, %t47
  br i1 %t49, label %dynarray.grow.11, label %dynarray.store.12
dynarray.grow.11:
  %t50 = icmp eq i64 %t47, 0
  %t51 = mul i64 %t47, 2
  %t52 = select i1 %t50, i64 1, i64 %t51
  %t53 = mul i64 %t52, 8
  %t54 = call ptr @realloc(ptr %t45, i64 %t53)
  store ptr %t54, ptr %t42
  store i64 %t52, ptr %t44
  br label %dynarray.store.12
dynarray.store.12:
  %t55 = load ptr, ptr %t42
  %t56 = getelementptr inbounds { i64 }, ptr %t55, i64 %t46
  store { i64 } %t41, ptr %t56
  store i64 %t48, ptr %t43
  %t57 = load ptr, ptr %local.4
  %t58 = insertvalue { ptr } poison, ptr %t57, 0
  %local.5 = alloca { ptr }
  store { ptr } %t58, ptr %local.5
  %t59 = call ptr @fn.0(ptr %local.5, i64 9)
  %t60 = icmp eq ptr %t59, null
  %t61 = icmp eq i1 %t60, 1
  br i1 %t61, label %on.body.14, label %on.end.13
on.body.14:
  ret i32 2
on.end.13:
  %t62 = load { i64 }, ptr %t59
  %t63 = extractvalue { i64 } %t62, 0
  %t64 = icmp ne i64 %t63, 9
  %t65 = icmp eq i1 %t64, 1
  br i1 %t65, label %on.body.16, label %on.end.15
on.body.16:
  ret i32 3
on.end.15:
  %t66 = call ptr @fn.0(ptr %local.5, i64 3)
  %t67 = icmp ne ptr %t66, null
  %t68 = icmp eq i1 %t67, 1
  br i1 %t68, label %on.body.18, label %on.end.17
on.body.18:
  ret i32 4
on.end.17:
  ret i32 0
}

@$find = internal alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
