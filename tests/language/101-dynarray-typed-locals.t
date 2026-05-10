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
  %local.3 = alloca i64
  store i64 0, ptr %local.3
  %local.4 = alloca i64
  store i64 0, ptr %local.4
  br label %for.cond.8
for.cond.8:
  %t27 = load i64, ptr %local.4
  %t28 = extractvalue { ptr, i64 } %sep, 1
  %t29 = add i64 %t27, %t28
  %t30 = extractvalue { ptr, i64 } %s, 1
  %t31 = icmp sle i64 %t29, %t30
  br i1 %t31, label %for.body.9, label %for.end.11
for.body.9:
  %local.5 = alloca i1
  store i1 1, ptr %local.5
  %local.6 = alloca i64
  store i64 0, ptr %local.6
  br label %for.cond.12
for.cond.12:
  %t32 = load i1, ptr %local.5
  %t33 = load i64, ptr %local.6
  %t34 = extractvalue { ptr, i64 } %sep, 1
  %t35 = icmp slt i64 %t33, %t34
  %t36 = and i1 %t32, %t35
  br i1 %t36, label %for.body.13, label %for.end.15
for.body.13:
  %t37 = extractvalue { ptr, i64 } %s, 0
  %t38 = load i64, ptr %local.4
  %t39 = load i64, ptr %local.6
  %t40 = add i64 %t38, %t39
  %t41 = getelementptr inbounds i8, ptr %t37, i64 %t40
  %t42 = load i8, ptr %t41
  %t43 = extractvalue { ptr, i64 } %sep, 0
  %t44 = load i64, ptr %local.6
  %t45 = getelementptr inbounds i8, ptr %t43, i64 %t44
  %t46 = load i8, ptr %t45
  %t47 = icmp ne i8 %t42, %t46
  %t48 = icmp eq i1 %t47, 1
  br i1 %t48, label %on.body.17, label %on.end.16
on.body.17:
  store i1 0, ptr %local.5
  br label %on.end.16
on.end.16:
  br label %for.update.14
for.update.14:
  %t49 = load i64, ptr %local.6
  %t50 = add i64 %t49, 1
  store i64 %t50, ptr %local.6
  br label %for.cond.12
for.end.15:
  %t51 = load i1, ptr %local.5
  %t52 = icmp eq i1 %t51, 1
  br i1 %t52, label %on.body.19, label %on.next.20
on.body.19:
  %t53 = alloca ptr, align 4
  store ptr null, ptr %t53, align 4
  %t54 = load ptr, ptr %local.2
  %t55 = icmp eq ptr %t54, null
  br i1 %t55, label %dynarray.alloc.22, label %dynarray.ready.23
dynarray.alloc.22:
  %t56 = call ptr @malloc(i64 24)
  %t57 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 0
  %t58 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 1
  %t59 = getelementptr inbounds { ptr, i64, i64 }, ptr %t56, i64 0, i32 2
  store ptr null, ptr %t57
  store i64 0, ptr %t58
  store i64 0, ptr %t59
  store ptr %t56, ptr %local.2
  br label %dynarray.ready.23
dynarray.ready.23:
  %t60 = load ptr, ptr %local.2
  %t61 = extractvalue { ptr, i64 } %s, 0
  %t62 = extractvalue { ptr, i64 } %s, 1
  %t63 = load i64, ptr %local.3
  %t64 = load i64, ptr %local.4
  %t65 = sub i64 %t64, %t63
  %t66 = getelementptr inbounds i8, ptr %t61, i64 %t63
  %t67 = insertvalue { ptr, i64 } poison, ptr %t66, 0
  %t68 = insertvalue { ptr, i64 } %t67, i64 %t65, 1
  %t69 = getelementptr inbounds { ptr, i64, i64 }, ptr %t60, i64 0, i32 0
  %t70 = getelementptr inbounds { ptr, i64, i64 }, ptr %t60, i64 0, i32 1
  %t71 = getelementptr inbounds { ptr, i64, i64 }, ptr %t60, i64 0, i32 2
  %t72 = load ptr, ptr %t69
  %t73 = load i64, ptr %t70
  %t74 = load i64, ptr %t71
  %t75 = add i64 %t73, 1
  %t76 = icmp ugt i64 %t75, %t74
  br i1 %t76, label %dynarray.grow.24, label %dynarray.store.25
dynarray.grow.24:
  %t77 = icmp eq i64 %t74, 0
  %t78 = mul i64 %t74, 2
  %t79 = select i1 %t77, i64 1, i64 %t78
  %t80 = mul i64 %t79, 16
  %t81 = call ptr @realloc(ptr %t72, i64 %t80)
  store ptr %t81, ptr %t69
  store i64 %t79, ptr %t71
  br label %dynarray.store.25
dynarray.store.25:
  %t82 = load ptr, ptr %t69
  %t83 = getelementptr inbounds { ptr, i64 }, ptr %t82, i64 %t73
  store { ptr, i64 } %t68, ptr %t83
  store i64 %t75, ptr %t70
  %t84 = load i64, ptr %local.4
  %t85 = extractvalue { ptr, i64 } %sep, 1
  %t86 = add i64 %t84, %t85
  store i64 %t86, ptr %local.4
  %t87 = load i64, ptr %local.4
  store i64 %t87, ptr %local.3
  br label %block.end.21
block.end.21:
  %t88 = load ptr, ptr %t53, align 4
  br label %on.end.18
on.next.20:
  br label %on.body.26
on.body.26:
  %t89 = alloca ptr, align 4
  store ptr null, ptr %t89, align 4
  %t90 = load i64, ptr %local.4
  %t91 = add i64 %t90, 1
  store i64 %t91, ptr %local.4
  br label %block.end.27
block.end.27:
  %t92 = load ptr, ptr %t89, align 4
  br label %on.end.18
on.end.18:
  br label %for.cond.8
for.end.11:
  %t93 = load ptr, ptr %local.2
  %t94 = icmp eq ptr %t93, null
  br i1 %t94, label %dynarray.alloc.28, label %dynarray.ready.29
dynarray.alloc.28:
  %t95 = call ptr @malloc(i64 24)
  %t96 = getelementptr inbounds { ptr, i64, i64 }, ptr %t95, i64 0, i32 0
  %t97 = getelementptr inbounds { ptr, i64, i64 }, ptr %t95, i64 0, i32 1
  %t98 = getelementptr inbounds { ptr, i64, i64 }, ptr %t95, i64 0, i32 2
  store ptr null, ptr %t96
  store i64 0, ptr %t97
  store i64 0, ptr %t98
  store ptr %t95, ptr %local.2
  br label %dynarray.ready.29
dynarray.ready.29:
  %t99 = load ptr, ptr %local.2
  %t100 = extractvalue { ptr, i64 } %s, 0
  %t101 = extractvalue { ptr, i64 } %s, 1
  %t102 = load i64, ptr %local.3
  %t103 = sub i64 %t101, %t102
  %t104 = getelementptr inbounds i8, ptr %t100, i64 %t102
  %t105 = insertvalue { ptr, i64 } poison, ptr %t104, 0
  %t106 = insertvalue { ptr, i64 } %t105, i64 %t103, 1
  %t107 = getelementptr inbounds { ptr, i64, i64 }, ptr %t99, i64 0, i32 0
  %t108 = getelementptr inbounds { ptr, i64, i64 }, ptr %t99, i64 0, i32 1
  %t109 = getelementptr inbounds { ptr, i64, i64 }, ptr %t99, i64 0, i32 2
  %t110 = load ptr, ptr %t107
  %t111 = load i64, ptr %t108
  %t112 = load i64, ptr %t109
  %t113 = add i64 %t111, 1
  %t114 = icmp ugt i64 %t113, %t112
  br i1 %t114, label %dynarray.grow.30, label %dynarray.store.31
dynarray.grow.30:
  %t115 = icmp eq i64 %t112, 0
  %t116 = mul i64 %t112, 2
  %t117 = select i1 %t115, i64 1, i64 %t116
  %t118 = mul i64 %t117, 16
  %t119 = call ptr @realloc(ptr %t110, i64 %t118)
  store ptr %t119, ptr %t107
  store i64 %t117, ptr %t109
  br label %dynarray.store.31
dynarray.store.31:
  %t120 = load ptr, ptr %t107
  %t121 = getelementptr inbounds { ptr, i64 }, ptr %t120, i64 %t111
  store { ptr, i64 } %t106, ptr %t121
  store i64 %t113, ptr %t108
  %t122 = load ptr, ptr %local.2
  ret ptr %t122
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
