split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    start: usize = 0
    i: usize = 0
    for i + sep.count <= s.count {
        matched := yes
        for j: usize = 0; matched && j < sep.count; j += 1 {
            on s.data[i + j] != sep.data[j] => matched = no
        }

        on matched => {
            parts.push(s[start .. i])
            i += sep.count
            start = i
        } else {
            i += 1
        }
    }

    parts.push(s[start ..])
    return parts
}

main :: fn () -> i32 {
    parts := split("look around", " ")
    on parts.count != 2 => return 1
    on parts[0] != "look" => return 2
    on parts[1] != "around" => return 3
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
  let start: usize = usize 0
  let i: usize = usize 0
  expr <unknown> for condition {
    condition <unknown> less_equal(<unknown> add(usize local.4(i), <unknown> field(string local.1(sep), count)), <unknown> field(string local.0(s), count))
    body {
      let matched: bool = bool yes
      expr <unknown> for c_style {
    init {
      let j: usize = usize 0
    }
    condition <unknown> logical_and(bool local.5(matched), <unknown> less(usize local.6(j), <unknown> field(string local.1(sep), count)))
    body {
      expr <unknown> on <unknown> not_equal(<unknown> index(<unknown> field(string local.0(s), data), <unknown> add(usize local.4(i), usize local.6(j))), <unknown> index(<unknown> field(string local.1(sep), data), usize local.6(j))) {
    value(<unknown> yes) => {
      assign bool local.5(matched) = <unknown> no
    }
  }
    }
    update {
      assign usize local.6(j) = <unknown> add(usize local.6(j), <unknown> 1)
    }
  }
      expr <unknown> on bool local.5(matched) {
    value(<unknown> yes) => {
      expr <unknown> block {
    expr <unknown> call <unknown> field([..]string local.2(parts), push)(<unknown> slice(string local.0(s), usize local.3(start), usize local.4(i)))
    assign usize local.4(i) = <unknown> add(usize local.4(i), <unknown> field(string local.1(sep), count))
    assign usize local.3(start) = usize local.4(i)
  }
    }
    else => {
      expr <unknown> block {
    assign usize local.4(i) = <unknown> add(usize local.4(i), <unknown> 1)
  }
    }
  }
    }
  }
  expr <unknown> call <unknown> field([..]string local.2(parts), push)(<unknown> slice(string local.0(s), usize local.3(start), <none>))
  return [..]string local.2(parts)
}
func fn.1() -> i32 {
  let parts: [..]string = [..]string call bind.0(split)(string "look around", string " ")
  expr void on bool not_equal(usize field([..]string local.7(parts), count), usize 2) {
    value(bool yes) => {
      return i32 1
    }
  }
  expr void on bool not_equal(string index([..]string local.7(parts), untyped integer 0), string "look") {
    value(bool yes) => {
      return i32 2
    }
  }
  expr void on bool not_equal(string index([..]string local.7(parts), untyped integer 1), string "around") {
    value(bool yes) => {
      return i32 3
    }
  }
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [12 x i8] c"look around\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"look\00"
@.str.m0.3 = private unnamed_addr constant [7 x i8] c"around\00"

declare i1 @string_eq({ ptr, i64 }, { ptr, i64 })
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string({ ptr, i64 })
declare void @string_builder_append_byte(i8)
declare { ptr, i64 } @string_builder_finish(i64)
declare { ptr, i64 } @to_string$string({ ptr, i64 })
declare { ptr, i64 } @to_string$bool(i1)
declare { ptr, i64 } @to_string$i8(i8)
declare { ptr, i64 } @to_string$i16(i16)
declare { ptr, i64 } @to_string$i32(i32)
declare { ptr, i64 } @to_string$i64(i64)
declare { ptr, i64 } @to_string$u8(i8)
declare { ptr, i64 } @to_string$u16(i16)
declare { ptr, i64 } @to_string$u32(i32)
declare { ptr, i64 } @to_string$u64(i64)
declare { ptr, i64 } @to_string$isize(i64)
declare { ptr, i64 } @to_string$usize(i64)
declare { ptr, i64 } @to_string$f32(float)
declare { ptr, i64 } @to_string$f64(double)
declare ptr @malloc(i64)
declare ptr @realloc(ptr, i64)
declare void @free(ptr)

define ptr @fn.0({ ptr, i64 } %s, { ptr, i64 } %sep) {
  %local.2 = alloca ptr
  store ptr null, ptr %local.2
  %t0 = extractvalue { ptr, i64 } %sep, 1
  %t1 = icmp eq i64 %t0, 0
  %t2 = icmp eq i1 %t1, 1
  br i1 %t2, label %on.body.1, label %on.end.0
on.body.1:
  %t3 = load ptr, ptr %local.2
  %t4 = icmp eq ptr %t3, null
  br i1 %t4, label %dynarray.alloc.3, label %dynarray.ready.4
dynarray.alloc.3:
  %t5 = call ptr @malloc(i64 24)
  %t6 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 0
  %t7 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 1
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t5, i64 0, i32 2
  store ptr null, ptr %t6
  store i64 0, ptr %t7
  store i64 0, ptr %t8
  store ptr %t5, ptr %local.2
  br label %dynarray.ready.4
dynarray.ready.4:
  %t9 = load ptr, ptr %local.2
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 0
  %t11 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 1
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 2
  %t13 = load ptr, ptr %t10
  %t14 = load i64, ptr %t11
  %t15 = load i64, ptr %t12
  %t16 = add i64 %t14, 1
  %t17 = icmp ugt i64 %t16, %t15
  br i1 %t17, label %dynarray.grow.5, label %dynarray.store.6
dynarray.grow.5:
  %t18 = icmp eq i64 %t15, 0
  %t19 = mul i64 %t15, 2
  %t20 = select i1 %t18, i64 1, i64 %t19
  %t21 = mul i64 %t20, 16
  %t22 = call ptr @realloc(ptr %t13, i64 %t21)
  store ptr %t22, ptr %t10
  store i64 %t20, ptr %t12
  br label %dynarray.store.6
dynarray.store.6:
  %t23 = load ptr, ptr %t10
  %t24 = getelementptr inbounds { ptr, i64 }, ptr %t23, i64 %t14
  store { ptr, i64 } %s, ptr %t24
  store i64 %t16, ptr %t11
  %t25 = load ptr, ptr %local.2
  ret ptr %t25
on.end.0:
  %local.3 = alloca i64
  store i64 0, ptr %local.3
  %local.4 = alloca i64
  store i64 0, ptr %local.4
  br label %for.cond.7
for.cond.7:
  %t26 = load i64, ptr %local.4
  %t27 = extractvalue { ptr, i64 } %sep, 1
  %t28 = add i64 %t26, %t27
  %t29 = extractvalue { ptr, i64 } %s, 1
  %t30 = icmp sle i64 %t28, %t29
  br i1 %t30, label %for.body.8, label %for.end.10
for.body.8:
  %local.5 = alloca i1
  store i1 1, ptr %local.5
  %local.6 = alloca i64
  store i64 0, ptr %local.6
  br label %for.cond.11
for.cond.11:
  %t31 = load i1, ptr %local.5
  %t32 = load i64, ptr %local.6
  %t33 = extractvalue { ptr, i64 } %sep, 1
  %t34 = icmp slt i64 %t32, %t33
  %t35 = and i1 %t31, %t34
  br i1 %t35, label %for.body.12, label %for.end.14
for.body.12:
  %t36 = extractvalue { ptr, i64 } %s, 0
  %t37 = load i64, ptr %local.4
  %t38 = load i64, ptr %local.6
  %t39 = add i64 %t37, %t38
  %t40 = getelementptr inbounds i8, ptr %t36, i64 %t39
  %t41 = load i8, ptr %t40
  %t42 = extractvalue { ptr, i64 } %sep, 0
  %t43 = load i64, ptr %local.6
  %t44 = getelementptr inbounds i8, ptr %t42, i64 %t43
  %t45 = load i8, ptr %t44
  %t46 = icmp ne i8 %t41, %t45
  %t47 = icmp eq i1 %t46, 1
  br i1 %t47, label %on.body.16, label %on.end.15
on.body.16:
  store i1 0, ptr %local.5
  br label %on.end.15
on.end.15:
  br label %for.update.13
for.update.13:
  %t48 = load i64, ptr %local.6
  %t49 = add i64 %t48, 1
  store i64 %t49, ptr %local.6
  br label %for.cond.11
for.end.14:
  %t50 = load i1, ptr %local.5
  %t51 = icmp eq i1 %t50, 1
  br i1 %t51, label %on.body.18, label %on.next.19
on.body.18:
  %t52 = load ptr, ptr %local.2
  %t53 = icmp eq ptr %t52, null
  br i1 %t53, label %dynarray.alloc.21, label %dynarray.ready.22
dynarray.alloc.21:
  %t54 = call ptr @malloc(i64 24)
  %t55 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 0
  %t56 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 1
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t54, i64 0, i32 2
  store ptr null, ptr %t55
  store i64 0, ptr %t56
  store i64 0, ptr %t57
  store ptr %t54, ptr %local.2
  br label %dynarray.ready.22
dynarray.ready.22:
  %t58 = load ptr, ptr %local.2
  %t59 = extractvalue { ptr, i64 } %s, 0
  %t60 = extractvalue { ptr, i64 } %s, 1
  %t61 = load i64, ptr %local.3
  %t62 = load i64, ptr %local.4
  %t63 = sub i64 %t62, %t61
  %t64 = getelementptr inbounds i8, ptr %t59, i64 %t61
  %t65 = insertvalue { ptr, i64 } poison, ptr %t64, 0
  %t66 = insertvalue { ptr, i64 } %t65, i64 %t63, 1
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t58, i64 0, i32 0
  %t68 = getelementptr inbounds { ptr, i64, i64 }, ptr %t58, i64 0, i32 1
  %t69 = getelementptr inbounds { ptr, i64, i64 }, ptr %t58, i64 0, i32 2
  %t70 = load ptr, ptr %t67
  %t71 = load i64, ptr %t68
  %t72 = load i64, ptr %t69
  %t73 = add i64 %t71, 1
  %t74 = icmp ugt i64 %t73, %t72
  br i1 %t74, label %dynarray.grow.23, label %dynarray.store.24
dynarray.grow.23:
  %t75 = icmp eq i64 %t72, 0
  %t76 = mul i64 %t72, 2
  %t77 = select i1 %t75, i64 1, i64 %t76
  %t78 = mul i64 %t77, 16
  %t79 = call ptr @realloc(ptr %t70, i64 %t78)
  store ptr %t79, ptr %t67
  store i64 %t77, ptr %t69
  br label %dynarray.store.24
dynarray.store.24:
  %t80 = load ptr, ptr %t67
  %t81 = getelementptr inbounds { ptr, i64 }, ptr %t80, i64 %t71
  store { ptr, i64 } %t66, ptr %t81
  store i64 %t73, ptr %t68
  %t82 = load i64, ptr %local.4
  %t83 = extractvalue { ptr, i64 } %sep, 1
  %t84 = add i64 %t82, %t83
  store i64 %t84, ptr %local.4
  %t85 = load i64, ptr %local.4
  store i64 %t85, ptr %local.3
  br label %block.end.20
block.end.20:
  br label %on.end.17
on.next.19:
  br label %on.body.25
on.body.25:
  %t86 = load i64, ptr %local.4
  %t87 = add i64 %t86, 1
  store i64 %t87, ptr %local.4
  br label %block.end.26
block.end.26:
  br label %on.end.17
on.end.17:
  br label %for.cond.7
for.end.10:
  %t88 = load ptr, ptr %local.2
  %t89 = icmp eq ptr %t88, null
  br i1 %t89, label %dynarray.alloc.27, label %dynarray.ready.28
dynarray.alloc.27:
  %t90 = call ptr @malloc(i64 24)
  %t91 = getelementptr inbounds { ptr, i64, i64 }, ptr %t90, i64 0, i32 0
  %t92 = getelementptr inbounds { ptr, i64, i64 }, ptr %t90, i64 0, i32 1
  %t93 = getelementptr inbounds { ptr, i64, i64 }, ptr %t90, i64 0, i32 2
  store ptr null, ptr %t91
  store i64 0, ptr %t92
  store i64 0, ptr %t93
  store ptr %t90, ptr %local.2
  br label %dynarray.ready.28
dynarray.ready.28:
  %t94 = load ptr, ptr %local.2
  %t95 = extractvalue { ptr, i64 } %s, 0
  %t96 = extractvalue { ptr, i64 } %s, 1
  %t97 = load i64, ptr %local.3
  %t98 = sub i64 %t96, %t97
  %t99 = getelementptr inbounds i8, ptr %t95, i64 %t97
  %t100 = insertvalue { ptr, i64 } poison, ptr %t99, 0
  %t101 = insertvalue { ptr, i64 } %t100, i64 %t98, 1
  %t102 = getelementptr inbounds { ptr, i64, i64 }, ptr %t94, i64 0, i32 0
  %t103 = getelementptr inbounds { ptr, i64, i64 }, ptr %t94, i64 0, i32 1
  %t104 = getelementptr inbounds { ptr, i64, i64 }, ptr %t94, i64 0, i32 2
  %t105 = load ptr, ptr %t102
  %t106 = load i64, ptr %t103
  %t107 = load i64, ptr %t104
  %t108 = add i64 %t106, 1
  %t109 = icmp ugt i64 %t108, %t107
  br i1 %t109, label %dynarray.grow.29, label %dynarray.store.30
dynarray.grow.29:
  %t110 = icmp eq i64 %t107, 0
  %t111 = mul i64 %t107, 2
  %t112 = select i1 %t110, i64 1, i64 %t111
  %t113 = mul i64 %t112, 16
  %t114 = call ptr @realloc(ptr %t105, i64 %t113)
  store ptr %t114, ptr %t102
  store i64 %t112, ptr %t104
  br label %dynarray.store.30
dynarray.store.30:
  %t115 = load ptr, ptr %t102
  %t116 = getelementptr inbounds { ptr, i64 }, ptr %t115, i64 %t106
  store { ptr, i64 } %t101, ptr %t116
  store i64 %t108, ptr %t103
  %t117 = load ptr, ptr %local.2
  ret ptr %t117
}

define i32 @fn.1() {
  %t0 = call ptr @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 11 }, { ptr, i64 } { ptr @.str.m0.1, i64 1 })
  %local.7 = alloca ptr
  store ptr %t0, ptr %local.7
  %t1 = load ptr, ptr %local.7
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
  %t7 = icmp ne i64 %t6, 2
  %t8 = icmp eq i1 %t7, 1
  br i1 %t8, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t9 = load ptr, ptr %local.7
  %t10 = getelementptr inbounds { ptr, i64, i64 }, ptr %t9, i64 0, i32 0
  %t11 = load ptr, ptr %t10
  %t12 = getelementptr inbounds { ptr, i64 }, ptr %t11, i32 0
  %t13 = load { ptr, i64 }, ptr %t12
  %t14 = call i1 @string_eq({ ptr, i64 } %t13, { ptr, i64 } { ptr @.str.m0.2, i64 4 })
  %t15 = xor i1 %t14, 1
  %t16 = icmp eq i1 %t15, 1
  br i1 %t16, label %on.body.6, label %on.end.5
on.body.6:
  ret i32 2
on.end.5:
  %t17 = load ptr, ptr %local.7
  %t18 = getelementptr inbounds { ptr, i64, i64 }, ptr %t17, i64 0, i32 0
  %t19 = load ptr, ptr %t18
  %t20 = getelementptr inbounds { ptr, i64 }, ptr %t19, i32 1
  %t21 = load { ptr, i64 }, ptr %t20
  %t22 = call i1 @string_eq({ ptr, i64 } %t21, { ptr, i64 } { ptr @.str.m0.3, i64 6 })
  %t23 = xor i1 %t22, 1
  %t24 = icmp eq i1 %t23, 1
  br i1 %t24, label %on.body.8, label %on.end.7
on.body.8:
  ret i32 3
on.end.7:
  ret i32 0
}

@$split = alias ptr ({ ptr, i64 }, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1