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
  %t0 = call ptr @malloc(i64 24)
  %t1 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 0
  %t2 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 1
  %t3 = getelementptr inbounds { ptr, i64, i64 }, ptr %t0, i64 0, i32 2
  %t4 = call ptr @malloc(i64 16)
  store ptr %t4, ptr %t1
  store i64 0, ptr %t2
  store i64 2, ptr %t3
  %local.3 = alloca ptr
  store ptr %t0, ptr %local.3
  %t5 = load ptr, ptr %local.3
  %t6 = icmp eq ptr %t5, null
  br i1 %t6, label %dynarray.alloc.0, label %dynarray.ready.1
dynarray.alloc.0:
  %t7 = call ptr @malloc(i64 24)
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 0
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 1
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 2
  store ptr null, ptr %t8
  store i64 0, ptr %t9
  store i64 0, ptr %t10
  store ptr %t7, ptr %local.3
  br label %dynarray.ready.1
dynarray.ready.1:
  %t11 = load ptr, ptr %local.3
  %t12 = insertvalue { i64 } poison, i64 7, 0
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 0
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 1
  %t15 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 2
  %t16 = load ptr, ptr %t13
  %t17 = load i64, ptr %t14
  %t18 = load i64, ptr %t15
  %t19 = add i64 %t17, 1
  %t20 = icmp ugt i64 %t19, %t18
  br i1 %t20, label %dynarray.grow.2, label %dynarray.store.3
dynarray.grow.2:
  %t21 = icmp eq i64 %t18, 0
  %t22 = mul i64 %t18, 2
  %t23 = select i1 %t21, i64 1, i64 %t22
  %t24 = mul i64 %t23, 8
  %t25 = call ptr @realloc(ptr %t16, i64 %t24)
  store ptr %t25, ptr %t13
  store i64 %t23, ptr %t15
  br label %dynarray.store.3
dynarray.store.3:
  %t26 = load ptr, ptr %t13
  %t27 = getelementptr inbounds { i64 }, ptr %t26, i64 %t17
  store { i64 } %t12, ptr %t27
  store i64 %t19, ptr %t14
  %t28 = load ptr, ptr %local.3
  %t29 = icmp eq ptr %t28, null
  br i1 %t29, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t30 = call ptr @malloc(i64 24)
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t30, i64 0, i32 0
  %t32 = getelementptr inbounds { ptr, i64, i64 }, ptr %t30, i64 0, i32 1
  %t33 = getelementptr inbounds { ptr, i64, i64 }, ptr %t30, i64 0, i32 2
  store ptr null, ptr %t31
  store i64 0, ptr %t32
  store i64 0, ptr %t33
  store ptr %t30, ptr %local.3
  br label %dynarray.ready.5
dynarray.ready.5:
  %t34 = load ptr, ptr %local.3
  %t35 = insertvalue { i64 } poison, i64 9, 0
  %t36 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 0
  %t37 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 1
  %t38 = getelementptr inbounds { ptr, i64, i64 }, ptr %t34, i64 0, i32 2
  %t39 = load ptr, ptr %t36
  %t40 = load i64, ptr %t37
  %t41 = load i64, ptr %t38
  %t42 = add i64 %t40, 1
  %t43 = icmp ugt i64 %t42, %t41
  br i1 %t43, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t44 = icmp eq i64 %t41, 0
  %t45 = mul i64 %t41, 2
  %t46 = select i1 %t44, i64 1, i64 %t45
  %t47 = mul i64 %t46, 8
  %t48 = call ptr @realloc(ptr %t39, i64 %t47)
  store ptr %t48, ptr %t36
  store i64 %t46, ptr %t38
  br label %dynarray.store.7
dynarray.store.7:
  %t49 = load ptr, ptr %t36
  %t50 = getelementptr inbounds { i64 }, ptr %t49, i64 %t40
  store { i64 } %t35, ptr %t50
  store i64 %t42, ptr %t37
  %t51 = load ptr, ptr %local.3
  %t52 = insertvalue { ptr } poison, ptr %t51, 0
  %local.4 = alloca { ptr }
  store { ptr } %t52, ptr %local.4
  %t53 = call ptr @fn.0(ptr %local.4, i64 9)
  %t54 = icmp eq ptr %t53, null
  %t55 = icmp eq i1 %t54, 1
  br i1 %t55, label %on.body.9, label %on.end.8
on.body.9:
  ret i32 1
on.end.8:
  %t56 = load { i64 }, ptr %t53
  %t57 = extractvalue { i64 } %t56, 0
  %t58 = icmp ne i64 %t57, 9
  %t59 = icmp eq i1 %t58, 1
  br i1 %t59, label %on.body.11, label %on.end.10
on.body.11:
  ret i32 2
on.end.10:
  %t60 = call ptr @fn.0(ptr %local.4, i64 3)
  %t61 = icmp ne ptr %t60, null
  %t62 = icmp eq i1 %t61, 1
  br i1 %t62, label %on.body.13, label %on.end.12
on.body.13:
  ret i32 3
on.end.12:
  ret i32 0
}

@$find = internal alias ptr (ptr, i64), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
