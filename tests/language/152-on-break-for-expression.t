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
  expr <unknown> <unsupported>
  let infos: [..]Info = <unknown> <unsupported>
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
  %local.3 = alloca ptr
  store ptr null, ptr %local.3
  %t0 = load ptr, ptr %local.3
  %t1 = icmp eq ptr %t0, null
  br i1 %t1, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t2 = call ptr @malloc(i64 24)
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 0
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 1
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t2, i64 0, i32 2
  store ptr null, ptr %t3
  store i64 0, ptr %t4
  store i64 0, ptr %t5
  store ptr %t2, ptr %local.3
  br label %dynarray.ready.1
dynarray.ready.1:
  %t6 = load ptr, ptr %local.3
  %t7 = insertvalue { i64 } poison, i64 7, 0
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 0
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  %t11 = load ptr, ptr %t8
  %t12 = load i64, ptr %t9
  %t13 = load i64, ptr %t10
  %t14 = add i64 %t12, 1
  %t15 = icmp ugt i64 %t14, %t13
  br i1 %t15, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t16 = icmp eq i64 %t13, 0
  %t17 = mul i64 %t13, 2
  %t18 = select i1 %t16, i64 1, i64 %t17
  %t19 = mul i64 %t18, 8
  %t20 = call ptr @realloc(ptr %t11, i64 %t19)
  store ptr %t20, ptr %t8
  store i64 %t18, ptr %t10
  br label %dynarray.store.3
dynarray.store.3:
  %t21 = load ptr, ptr %t8
  %t22 = getelementptr inbounds { i64 }, ptr %t21, i64 %t12
  store { i64 } %t7, ptr %t22
  store i64 %t14, ptr %t9
  %t23 = load ptr, ptr %local.3
  %t24 = icmp eq ptr %t23, null
  br i1 %t24, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t25 = call ptr @malloc(i64 24)
  %t26 = getelementptr inbounds { ptr, i64, i64 }, ptr %t25, i64 0, i32 0
  %t27 = getelementptr inbounds { ptr, i64, i64 }, ptr %t25, i64 0, i32 1
  %t28 = getelementptr inbounds { ptr, i64, i64 }, ptr %t25, i64 0, i32 2
  store ptr null, ptr %t26
  store i64 0, ptr %t27
  store i64 0, ptr %t28
  store ptr %t25, ptr %local.3
  br label %dynarray.ready.5
dynarray.ready.5:
  %t29 = load ptr, ptr %local.3
  %t30 = insertvalue { i64 } poison, i64 9, 0
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t29, i64 0, i32 0
  %t32 = getelementptr inbounds { ptr, i64, i64 }, ptr %t29, i64 0, i32 1
  %t33 = getelementptr inbounds { ptr, i64, i64 }, ptr %t29, i64 0, i32 2
  %t34 = load ptr, ptr %t31
  %t35 = load i64, ptr %t32
  %t36 = load i64, ptr %t33
  %t37 = add i64 %t35, 1
  %t38 = icmp ugt i64 %t37, %t36
  br i1 %t38, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t39 = icmp eq i64 %t36, 0
  %t40 = mul i64 %t36, 2
  %t41 = select i1 %t39, i64 1, i64 %t40
  %t42 = mul i64 %t41, 8
  %t43 = call ptr @realloc(ptr %t34, i64 %t42)
  store ptr %t43, ptr %t31
  store i64 %t41, ptr %t33
  br label %dynarray.store.7
dynarray.store.7:
  %t44 = load ptr, ptr %t31
  %t45 = getelementptr inbounds { i64 }, ptr %t44, i64 %t35
  store { i64 } %t30, ptr %t45
  store i64 %t37, ptr %t32
  %t46 = load ptr, ptr %local.3
  %t47 = insertvalue { ptr } poison, ptr %t46, 0
  %local.4 = alloca { ptr }
  store { ptr } %t47, ptr %local.4
  %t48 = call ptr @fn.0(ptr %local.4, i64 9)
  %t49 = icmp eq ptr %t48, null
  %t50 = icmp eq i1 %t49, 1
  br i1 %t50, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 1
on.end.8:
  %t51 = load { i64 }, ptr %t48
  %t52 = extractvalue { i64 } %t51, 0
  %t53 = icmp ne i64 %t52, 9
  %t54 = icmp eq i1 %t53, 1
  br i1 %t54, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 2
on.end.10:
  %t55 = call ptr @fn.0(ptr %local.4, i64 3)
  %t56 = icmp ne ptr %t55, null
  %t57 = icmp eq i1 %t56, 1
  br i1 %t57, label %on.body.13, label %on.end.12
on.body.13:
  ret i32 3
on.end.12:
  ret i32 0
}

@$find = alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
