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
  expr <unknown> <unsupported>
  let parts: [..]string = <unknown> <unsupported>
  expr <unknown> on <unknown> equal(<unknown> field(string local.1(sep), count), <unknown> 0) {
    value(<unknown> yes) => {
      expr <unknown> block {
    expr void call fn (string) -> void field([..]string local.2(parts), push)(string local.0(s))
    return [..]string local.2(parts)
  }
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
  expr <unknown> on <unknown> equal(<unknown> field([..]string local.3(parts), count), <unknown> 2) {
    value(<unknown> yes) => {
      expr <unknown> block {
    return i32 2
  }
    }
    else => {
      return <unknown> 0
    }
  }
  return <unknown> 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [5 x i8] c"look\00"
@.str.m0.1 = private unnamed_addr constant [1 x i8] c"\00"

declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

define ptr @fn.0({ ptr, i64 } %s, { ptr, i64 } %sep) {
  %local.2 = alloca ptr
  store ptr null, ptr %local.2
  %t0 = extractvalue { ptr, i64 } %sep, 1
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  %t3 = alloca ptr, align 4
  store ptr null, ptr %t3, align 4
  %t4 = load ptr, ptr %local.2
  %t5 = icmp eq ptr %t4, null
  br i1 %t5, label %dynarray.alloc.4, label %dynarray.ready.5
dynarray.alloc.4:
  %t6 = call ptr @malloc(i64 24)
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 0
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 1
  %t9 = getelementptr inbounds { ptr, i64, i64 }, ptr %t6, i64 0, i32 2
  store ptr null, ptr %t7
  store i64 0, ptr %t8
  store i64 0, ptr %t9
  store ptr %t6, ptr %local.2
  br label %dynarray.ready.5
dynarray.ready.5:
  %t10 = load ptr, ptr %local.2
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t10, i64 0, i32 0
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t10, i64 0, i32 1
  %t13 = getelementptr inbounds { ptr, i64, i64 }, ptr %t10, i64 0, i32 2
  %t14 = load ptr, ptr %t11
  %t15 = load i64, ptr %t12
  %t16 = load i64, ptr %t13
  %t17 = add i64 %t15, 1
  %t18 = icmp ugt i64 %t17, %t16
  br i1 %t18, label %dynarray.grow.6, label %dynarray.store.7
dynarray.grow.6:
  %t19 = icmp eq i64 %t16, 0
  %t20 = mul i64 %t16, 2
  %t21 = select i1 %t19, i64 1, i64 %t20
  %t22 = mul i64 %t21, 16
  %t23 = call ptr @realloc(ptr %t14, i64 %t22)
  store ptr %t23, ptr %t11
  store i64 %t21, ptr %t13
  br label %dynarray.store.7
dynarray.store.7:
  %t24 = load ptr, ptr %t11
  %t25 = getelementptr inbounds { ptr, i64 }, ptr %t24, i64 %t15
  store { ptr, i64 } %s, ptr %t25
  store i64 %t17, ptr %t12
  %t26 = load ptr, ptr %local.2
  ret ptr %t26
on.next.2:
  %t27 = load ptr, ptr %local.2
  ret ptr %t27
}

define i32 @fn.1() {
  %t0 = call ptr @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 4 }, { ptr, i64 } { ptr @.str.m0.1, i64 0 })
  %local.3 = alloca ptr
  store ptr %t0, ptr %local.3
  %t1 = load ptr, ptr %local.3
  %t2 = alloca i64
  %t3 = icmp eq ptr %t1, null
  br i1 %t3, label %dynarray.field.empty.0, label %dynarray.field.load.1
dynarray.field.empty.0:
  store i64 0, ptr %t2
  br label %dynarray.field.done.2
dynarray.field.load.1:
  %t4 = getelementptr inbounds { ptr, i64, i64 }, ptr %t1, i64 0, i32 1
  %t5 = load i64, ptr %t4
  store i64 %t5, ptr %t2
  br label %dynarray.field.done.2
dynarray.field.done.2:
  %t6 = load i64, ptr %t2
  %t7 = icmp ne i64 %t6, 1
  %t8 = icmp eq i1 %t7, 1
  br i1 %t8, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t9 = load ptr, ptr %local.3
  %t10 = alloca i64
  %t11 = icmp eq ptr %t9, null
  br i1 %t11, label %dynarray.field.empty.5, label %dynarray.field.load.6
dynarray.field.empty.5:
  store i64 0, ptr %t10
  br label %dynarray.field.done.7
dynarray.field.load.6:
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 1
  %t13 = load i64, ptr %t12
  store i64 %t13, ptr %t10
  br label %dynarray.field.done.7
dynarray.field.done.7:
  %t14 = load i64, ptr %t10
  %t15 = icmp eq i64 %t14, 2
  %t16 = icmp eq i1 %t15, 1
  br i1 %t16, label %on.body.9, label %on.next.10
on.body.9:
  %t17 = alloca ptr, align 4
  store ptr null, ptr %t17, align 4
  ret i32 2
on.next.10:
  br label %on.body.12
on.body.12:
  ret i32 0
}

@$split = alias ptr ({ ptr, i64 }, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
