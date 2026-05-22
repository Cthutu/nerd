split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    return parts
}

main :: fn () -> i32 {
    parts := split("look", "")
    on parts.count != 1 => return 1
    on parts.count == 2 => {
        return 2
    } else return 0
    return 0
}
¬
0
¬

¬
hir 0
bind split = fn.0
bind main = fn.1
func fn.0(s: string, sep: string) -> [..]string {
  expr <unknown> default
  let parts: [..]string = <unknown> default
  expr void on bool equal(usize field(string local.1(sep), count), usize 0) {
    value(bool yes) => {
      expr void call fn (string) -> void field([..]string local.2(parts), push)(string local.0(s))
      return [..]string local.2(parts)
    }
  }
  return [..]string local.2(parts)
}
func fn.1() -> i32 {
  let parts: [..]string = [..]string call bind.0(split)(string "look", string "")
  expr void on bool not_equal(usize field([..]string local.3(parts), count), usize 1) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool equal(usize field([..]string local.3(parts), count), usize 2) {
    value(bool yes) => {
      return i32 2
    }
    else => {
      return i32 0
    }
  }
  return <unknown> 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/100-on-short-block-expr.t\00"
@.str.m0.0 = private unnamed_addr constant [5 x i8] c"look\00"
@.str.m0.1 = private unnamed_addr constant [1 x i8] c"\00"

declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

define internal ptr @fn.0({ ptr, i64 } %s, { ptr, i64 } %sep) {
  %local.2 = alloca ptr
  store ptr null, ptr %local.2
  %t0 = extractvalue { ptr, i64 } %sep, 1
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  %t3 = load ptr, ptr %local.2
  %t4 = icmp eq ptr %t3, null
  br i1 %t4, label %dynarray.alloc.2, label %dynarray.ready.3
dynarray.alloc.2:
  %t5 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 5)
  %t6 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 0
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 1
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 2
  %t9 = getelementptr inbounds i8, ptr %t5, i64 24
  store ptr %t9, ptr %t6
  store i64 0, ptr %t7
  store i64 0, ptr %t8
  store ptr %t9, ptr %local.2
  br label %dynarray.ready.3
dynarray.ready.3:
  %t10 = load ptr, ptr %local.2
  %t11 = getelementptr inbounds i8, ptr %t10, i64 -24
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 0
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 1
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 2
  %t15 = load ptr, ptr %t12
  %t16 = load i64, ptr %t13
  %t17 = load i64, ptr %t14
  %t18 = add i64 %t16, 1
  %t19 = icmp ugt i64 %t18, %t17
  br i1 %t19, label %dynarray.grow.4, label %dynarray.store.5
dynarray.grow.4:
  %t20 = icmp eq i64 %t17, 0
  %t21 = mul i64 %t17, 2
  %t22 = select i1 %t20, i64 1, i64 %t21
  %t23 = mul i64 %t22, 16
  %t24 = add i64 24, %t23
  %t25 = call ptr @nrt_mem_realloc(ptr %t11, i64 %t24, i64 16, ptr @.macro.file.m0, i32 5)
  %t26 = getelementptr inbounds i8, ptr %t25, i64 24
  %t27 = getelementptr inbounds { ptr, i64, i64 }, ptr %t25, i64 0, i32 0
  %t28 = getelementptr inbounds { ptr, i64, i64 }, ptr %t25, i64 0, i32 2
  store ptr %t26, ptr %t27
  store i64 %t22, ptr %t28
  store ptr %t26, ptr %local.2
  br label %dynarray.store.5
dynarray.store.5:
  %t29 = load ptr, ptr %local.2
  %t30 = getelementptr inbounds i8, ptr %t29, i64 -24
  %t31 = getelementptr inbounds { ptr, i64, i64 }, ptr %t30, i64 0, i32 0
  %t32 = getelementptr inbounds { ptr, i64, i64 }, ptr %t30, i64 0, i32 1
  %t33 = load ptr, ptr %t31
  %t34 = getelementptr inbounds { ptr, i64 }, ptr %t33, i64 %t16
  store { ptr, i64 } %s, ptr %t34
  store i64 %t18, ptr %t32
  %t35 = load ptr, ptr %local.2
  ret ptr %t35
on.end.0:
  %t36 = load ptr, ptr %local.2
  ret ptr %t36
}

define internal i32 @fn.1() {
  %local.3 = alloca ptr
  %t0 = call ptr @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 4 }, { ptr, i64 } { ptr @.str.m0.1, i64 0 })
  store ptr %t0, ptr %local.3
  %t1 = load ptr, ptr %local.3
  %t2 = alloca i64
  %t3 = icmp eq ptr %t1, null
  br i1 %t3, label %dynarray.field.empty.0, label %dynarray.field.load.1
dynarray.field.empty.0:
  store i64 0, ptr %t2
  br label %dynarray.field.done.2
dynarray.field.load.1:
  %t4 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t4, i64 0, i32 1
  %t6 = load i64, ptr %t5
  store i64 %t6, ptr %t2
  br label %dynarray.field.done.2
dynarray.field.done.2:
  %t7 = load i64, ptr %t2
  %t8 = icmp ne i64 %t7, 1
  %t9 = icmp eq i1 %t8, 1
  br i1 %t9, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t10 = load ptr, ptr %local.3
  %t11 = alloca i64
  %t12 = icmp eq ptr %t10, null
  br i1 %t12, label %dynarray.field.empty.5, label %dynarray.field.load.6
dynarray.field.empty.5:
  store i64 0, ptr %t11
  br label %dynarray.field.done.7
dynarray.field.load.6:
  %t13 = getelementptr inbounds i8, ptr %t10, i64 -24
  %t14 = getelementptr inbounds { ptr, i64, i64 }, ptr %t13, i64 0, i32 1
  %t15 = load i64, ptr %t14
  store i64 %t15, ptr %t11
  br label %dynarray.field.done.7
dynarray.field.done.7:
  %t16 = load i64, ptr %t11
  %t17 = icmp eq i64 %t16, 2
  %t18 = icmp eq i1 %t17, 1
  br i1 %t18, label %on.body.9, label %on.next.10
on.body.9:
  ret i32 2
on.next.10:
  br label %on.body.11
on.body.11:
  ret i32 0
on.end.8:
  ret i32 0
}

@$split = internal alias ptr ({ ptr, i64 }, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
