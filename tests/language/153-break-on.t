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
  expr <unknown> <unsupported>
  let infos: [..]Info = <unknown> <unsupported>
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

define ptr @fn.0(ptr %system, i64 %handle) {
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

define i32 @fn.1() {
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
  %local.4 = alloca ptr
  store ptr null, ptr %local.4
  %t6 = load ptr, ptr %local.4
  %t7 = icmp eq ptr %t6, null
  br i1 %t7, label %dynarray.alloc.5, label %dynarray.ready.6
dynarray.alloc.5:
  %t8 = call ptr @malloc(i64 24)
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 0
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 1
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t8, i64 0, i32 2
  store ptr null, ptr %t9
  store i64 0, ptr %t10
  store i64 0, ptr %t11
  store ptr %t8, ptr %local.4
  br label %dynarray.ready.6
dynarray.ready.6:
  %t12 = load ptr, ptr %local.4
  %t13 = insertvalue { i64 } poison, i64 7, 0
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 0
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 1
  %t16 = getelementptr inbounds { ptr, i64, i64 }, ptr %t12, i64 0, i32 2
  %t17 = load ptr, ptr %t14
  %t18 = load i64, ptr %t15
  %t19 = load i64, ptr %t16
  %t20 = add i64 %t18, 1
  %t21 = icmp ugt i64 %t20, %t19
  br i1 %t21, label %dynarray.grow.7, label %dynarray.store.8
dynarray.grow.7:
  %t22 = icmp eq i64 %t19, 0
  %t23 = mul i64 %t19, 2
  %t24 = select i1 %t22, i64 1, i64 %t23
  %t25 = mul i64 %t24, 8
  %t26 = call ptr @realloc(ptr %t17, i64 %t25)
  store ptr %t26, ptr %t14
  store i64 %t24, ptr %t16
  br label %dynarray.store.8
dynarray.store.8:
  %t27 = load ptr, ptr %t14
  %t28 = getelementptr inbounds { i64 }, ptr %t27, i64 %t18
  store { i64 } %t13, ptr %t28
  store i64 %t20, ptr %t15
  %t29 = load ptr, ptr %local.4
  %t30 = icmp eq ptr %t29, null
  br i1 %t30, label %dynarray.alloc.9, label %dynarray.ready.10
dynarray.alloc.9:
  %t31 = call ptr @malloc(i64 24)
  %t32 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 0
  %t33 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 1
  %t34 = getelementptr inbounds { ptr, i64, i64 }, ptr %t31, i64 0, i32 2
  store ptr null, ptr %t32
  store i64 0, ptr %t33
  store i64 0, ptr %t34
  store ptr %t31, ptr %local.4
  br label %dynarray.ready.10
dynarray.ready.10:
  %t35 = load ptr, ptr %local.4
  %t36 = insertvalue { i64 } poison, i64 9, 0
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t35, i64 0, i32 0
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t35, i64 0, i32 1
  %t39 = getelementptr inbounds { ptr, i64, i64 }, ptr %t35, i64 0, i32 2
  %t40 = load ptr, ptr %t37
  %t41 = load i64, ptr %t38
  %t42 = load i64, ptr %t39
  %t43 = add i64 %t41, 1
  %t44 = icmp ugt i64 %t43, %t42
  br i1 %t44, label %dynarray.grow.11, label %dynarray.store.12
dynarray.grow.11:
  %t45 = icmp eq i64 %t42, 0
  %t46 = mul i64 %t42, 2
  %t47 = select i1 %t45, i64 1, i64 %t46
  %t48 = mul i64 %t47, 8
  %t49 = call ptr @realloc(ptr %t40, i64 %t48)
  store ptr %t49, ptr %t37
  store i64 %t47, ptr %t39
  br label %dynarray.store.12
dynarray.store.12:
  %t50 = load ptr, ptr %t37
  %t51 = getelementptr inbounds { i64 }, ptr %t50, i64 %t41
  store { i64 } %t36, ptr %t51
  store i64 %t43, ptr %t38
  %t52 = load ptr, ptr %local.4
  %t53 = insertvalue { ptr } poison, ptr %t52, 0
  %local.5 = alloca { ptr }
  store { ptr } %t53, ptr %local.5
  %t54 = call ptr @fn.0(ptr %local.5, i64 9)
  %t55 = icmp eq ptr %t54, null
  %t56 = icmp eq i1 %t55, 1
  br i1 %t56, label %on.body.14, label %on.end.13
on.body.14:
  ret i32 2
on.end.13:
  %t57 = load { i64 }, ptr %t54
  %t58 = extractvalue { i64 } %t57, 0
  %t59 = icmp ne i64 %t58, 9
  %t60 = icmp eq i1 %t59, 1
  br i1 %t60, label %on.body.16, label %on.end.15
on.body.16:
  ret i32 3
on.end.15:
  %t61 = call ptr @fn.0(ptr %local.5, i64 3)
  %t62 = icmp ne ptr %t61, null
  %t63 = icmp eq i1 %t62, 1
  br i1 %t63, label %on.body.18, label %on.end.17
on.body.18:
  ret i32 4
on.end.17:
  ret i32 0
}

@$find = alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
