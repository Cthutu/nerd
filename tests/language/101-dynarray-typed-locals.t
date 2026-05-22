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
  expr <unknown> default
  let parts: [..]string = <unknown> default
  expr void on bool equal(usize field(string local.1(sep), count), usize 0) {
    value(bool yes) => {
      expr void call fn (string) -> void field([..]string local.2(parts), push)(string local.0(s))
      return [..]string local.2(parts)
    }
  }
  let start: usize = usize 0
  let i: usize = usize 0
  expr void for condition {
    condition bool less_equal(usize add(usize local.4(i), usize field(string local.1(sep), count)), usize field(string local.0(s), count))
    body {
      let matched: bool = bool yes
      expr void for c_style {
    init {
      let j: usize = usize 0
    }
    condition bool logical_and(bool local.5(matched), bool less(usize local.6(j), usize field(string local.1(sep), count)))
    body {
      expr void on bool not_equal(u8 index(^u8 field(string local.0(s), data), usize add(usize local.4(i), usize local.6(j))), u8 index(^u8 field(string local.1(sep), data), usize local.6(j))) {
    value(bool yes) => {
      assign bool local.5(matched) = bool no
    }
  }
    }
    update {
      assign usize local.6(j) = usize add(usize local.6(j), usize 1)
    }
  }
      expr void on bool local.5(matched) {
    value(bool yes) => {
      expr void call fn (string) -> void field([..]string local.2(parts), push)(string slice(string local.0(s), usize local.3(start), usize local.4(i)))
      assign usize local.4(i) = usize add(usize local.4(i), usize field(string local.1(sep), count))
      assign usize local.3(start) = usize local.4(i)
    }
    else => {
      assign usize local.4(i) = usize add(usize local.4(i), usize 1)
    }
  }
    }
  }
  expr void call fn (string) -> void field([..]string local.2(parts), push)(string slice(string local.0(s), usize local.3(start), <none>))
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

@.macro.file.m0 = private unnamed_addr constant [43 x i8] c"tests/language/101-dynarray-typed-locals.t\00"
@.str.m0.0 = private unnamed_addr constant [12 x i8] c"look around\00"
@.str.m0.1 = private unnamed_addr constant [2 x i8] c" \00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"look\00"
@.str.m0.3 = private unnamed_addr constant [7 x i8] c"around\00"

declare i1 @string_eq(ptr, ptr)
declare void @string_builder_reset()
declare i64 @string_builder_mark()
declare void @string_builder_append_string(ptr)
declare void @string_builder_append_byte(i8)
declare void @string_builder_finish(ptr, i64)
declare void @to_string$string(ptr, ptr)
declare void @to_string$bool(ptr, i1)
declare void @to_string$i8(ptr, i8)
declare void @to_string$i16(ptr, i16)
declare void @to_string$i32(ptr, i32)
declare void @to_string$i64(ptr, i64)
declare void @to_string$u8(ptr, i8)
declare void @to_string$u16(ptr, i16)
declare void @to_string$u32(ptr, i32)
declare void @to_string$u64(ptr, i64)
declare void @to_string$isize(ptr, i64)
declare void @to_string$usize(ptr, i64)
declare void @to_string$f32(ptr, float)
declare void @to_string$f64(ptr, double)
declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

define internal ptr @fn.0({ ptr, i64 } %s, { ptr, i64 } %sep) {
  %local.2 = alloca ptr
  %local.3 = alloca i64
  %local.4 = alloca i64
  %local.5 = alloca i1
  %local.6 = alloca i64
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
  store i64 0, ptr %local.3
  store i64 0, ptr %local.4
  br label %for.cond.6
for.cond.6:
  %t36 = load i64, ptr %local.4
  %t37 = extractvalue { ptr, i64 } %sep, 1
  %t38 = add i64 %t36, %t37
  %t39 = extractvalue { ptr, i64 } %s, 1
  %t40 = icmp ule i64 %t38, %t39
  br i1 %t40, label %for.body.7, label %for.end.9
for.body.7:
  store i1 1, ptr %local.5
  store i64 0, ptr %local.6
  br label %for.cond.10
for.cond.10:
  %t41 = load i1, ptr %local.5
  %t42 = load i64, ptr %local.6
  %t43 = extractvalue { ptr, i64 } %sep, 1
  %t44 = icmp ult i64 %t42, %t43
  %t45 = and i1 %t41, %t44
  br i1 %t45, label %for.body.11, label %for.end.13
for.body.11:
  %t46 = extractvalue { ptr, i64 } %s, 0
  %t47 = load i64, ptr %local.4
  %t48 = load i64, ptr %local.6
  %t49 = add i64 %t47, %t48
  %t50 = getelementptr inbounds i8, ptr %t46, i64 %t49
  %t51 = load i8, ptr %t50
  %t52 = extractvalue { ptr, i64 } %sep, 0
  %t53 = load i64, ptr %local.6
  %t54 = getelementptr inbounds i8, ptr %t52, i64 %t53
  %t55 = load i8, ptr %t54
  %t56 = icmp ne i8 %t51, %t55
  %t57 = icmp eq i1 %t56, 1
  br i1 %t57, label %on.body.15, label %on.end.14
on.body.15:
  store i1 0, ptr %local.5
  br label %on.end.14
on.end.14:
  br label %for.update.12
for.update.12:
  %t58 = load i64, ptr %local.6
  %t59 = add i64 %t58, 1
  store i64 %t59, ptr %local.6
  br label %for.cond.10
for.end.13:
  %t60 = load i1, ptr %local.5
  %t61 = icmp eq i1 %t60, 1
  br i1 %t61, label %on.body.17, label %on.next.18
on.body.17:
  %t62 = load ptr, ptr %local.2
  %t63 = icmp eq ptr %t62, null
  br i1 %t63, label %dynarray.alloc.19, label %dynarray.ready.20
dynarray.alloc.19:
  %t64 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 18)
  %t65 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 0
  %t66 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 1
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t64, i64 0, i32 2
  %t68 = getelementptr inbounds i8, ptr %t64, i64 24
  store ptr %t68, ptr %t65
  store i64 0, ptr %t66
  store i64 0, ptr %t67
  store ptr %t68, ptr %local.2
  br label %dynarray.ready.20
dynarray.ready.20:
  %t69 = load ptr, ptr %local.2
  %t70 = getelementptr inbounds i8, ptr %t69, i64 -24
  %t71 = extractvalue { ptr, i64 } %s, 0
  %t72 = extractvalue { ptr, i64 } %s, 1
  %t73 = load i64, ptr %local.3
  %t74 = load i64, ptr %local.4
  %t75 = sub i64 %t74, %t73
  %t76 = getelementptr inbounds i8, ptr %t71, i64 %t73
  %t77 = insertvalue { ptr, i64 } poison, ptr %t76, 0
  %t78 = insertvalue { ptr, i64 } %t77, i64 %t75, 1
  %t79 = getelementptr inbounds { ptr, i64, i64 }, ptr %t70, i64 0, i32 0
  %t80 = getelementptr inbounds { ptr, i64, i64 }, ptr %t70, i64 0, i32 1
  %t81 = getelementptr inbounds { ptr, i64, i64 }, ptr %t70, i64 0, i32 2
  %t82 = load ptr, ptr %t79
  %t83 = load i64, ptr %t80
  %t84 = load i64, ptr %t81
  %t85 = add i64 %t83, 1
  %t86 = icmp ugt i64 %t85, %t84
  br i1 %t86, label %dynarray.grow.21, label %dynarray.store.22
dynarray.grow.21:
  %t87 = icmp eq i64 %t84, 0
  %t88 = mul i64 %t84, 2
  %t89 = select i1 %t87, i64 1, i64 %t88
  %t90 = mul i64 %t89, 16
  %t91 = add i64 24, %t90
  %t92 = call ptr @nrt_mem_realloc(ptr %t70, i64 %t91, i64 16, ptr @.macro.file.m0, i32 18)
  %t93 = getelementptr inbounds i8, ptr %t92, i64 24
  %t94 = getelementptr inbounds { ptr, i64, i64 }, ptr %t92, i64 0, i32 0
  %t95 = getelementptr inbounds { ptr, i64, i64 }, ptr %t92, i64 0, i32 2
  store ptr %t93, ptr %t94
  store i64 %t89, ptr %t95
  store ptr %t93, ptr %local.2
  br label %dynarray.store.22
dynarray.store.22:
  %t96 = load ptr, ptr %local.2
  %t97 = getelementptr inbounds i8, ptr %t96, i64 -24
  %t98 = getelementptr inbounds { ptr, i64, i64 }, ptr %t97, i64 0, i32 0
  %t99 = getelementptr inbounds { ptr, i64, i64 }, ptr %t97, i64 0, i32 1
  %t100 = load ptr, ptr %t98
  %t101 = getelementptr inbounds { ptr, i64 }, ptr %t100, i64 %t83
  store { ptr, i64 } %t78, ptr %t101
  store i64 %t85, ptr %t99
  %t102 = load i64, ptr %local.4
  %t103 = extractvalue { ptr, i64 } %sep, 1
  %t104 = add i64 %t102, %t103
  store i64 %t104, ptr %local.4
  %t105 = load i64, ptr %local.4
  store i64 %t105, ptr %local.3
  br label %on.end.16
on.next.18:
  br label %on.body.23
on.body.23:
  %t106 = load i64, ptr %local.4
  %t107 = add i64 %t106, 1
  store i64 %t107, ptr %local.4
  br label %on.end.16
on.end.16:
  br label %for.cond.6
for.end.9:
  %t108 = load ptr, ptr %local.2
  %t109 = icmp eq ptr %t108, null
  br i1 %t109, label %dynarray.alloc.24, label %dynarray.ready.25
dynarray.alloc.24:
  %t110 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 26)
  %t111 = getelementptr inbounds { ptr, i64, i64 }, ptr %t110, i64 0, i32 0
  %t112 = getelementptr inbounds { ptr, i64, i64 }, ptr %t110, i64 0, i32 1
  %t113 = getelementptr inbounds { ptr, i64, i64 }, ptr %t110, i64 0, i32 2
  %t114 = getelementptr inbounds i8, ptr %t110, i64 24
  store ptr %t114, ptr %t111
  store i64 0, ptr %t112
  store i64 0, ptr %t113
  store ptr %t114, ptr %local.2
  br label %dynarray.ready.25
dynarray.ready.25:
  %t115 = load ptr, ptr %local.2
  %t116 = getelementptr inbounds i8, ptr %t115, i64 -24
  %t117 = extractvalue { ptr, i64 } %s, 0
  %t118 = extractvalue { ptr, i64 } %s, 1
  %t119 = load i64, ptr %local.3
  %t120 = sub i64 %t118, %t119
  %t121 = getelementptr inbounds i8, ptr %t117, i64 %t119
  %t122 = insertvalue { ptr, i64 } poison, ptr %t121, 0
  %t123 = insertvalue { ptr, i64 } %t122, i64 %t120, 1
  %t124 = getelementptr inbounds { ptr, i64, i64 }, ptr %t116, i64 0, i32 0
  %t125 = getelementptr inbounds { ptr, i64, i64 }, ptr %t116, i64 0, i32 1
  %t126 = getelementptr inbounds { ptr, i64, i64 }, ptr %t116, i64 0, i32 2
  %t127 = load ptr, ptr %t124
  %t128 = load i64, ptr %t125
  %t129 = load i64, ptr %t126
  %t130 = add i64 %t128, 1
  %t131 = icmp ugt i64 %t130, %t129
  br i1 %t131, label %dynarray.grow.26, label %dynarray.store.27
dynarray.grow.26:
  %t132 = icmp eq i64 %t129, 0
  %t133 = mul i64 %t129, 2
  %t134 = select i1 %t132, i64 1, i64 %t133
  %t135 = mul i64 %t134, 16
  %t136 = add i64 24, %t135
  %t137 = call ptr @nrt_mem_realloc(ptr %t116, i64 %t136, i64 16, ptr @.macro.file.m0, i32 26)
  %t138 = getelementptr inbounds i8, ptr %t137, i64 24
  %t139 = getelementptr inbounds { ptr, i64, i64 }, ptr %t137, i64 0, i32 0
  %t140 = getelementptr inbounds { ptr, i64, i64 }, ptr %t137, i64 0, i32 2
  store ptr %t138, ptr %t139
  store i64 %t134, ptr %t140
  store ptr %t138, ptr %local.2
  br label %dynarray.store.27
dynarray.store.27:
  %t141 = load ptr, ptr %local.2
  %t142 = getelementptr inbounds i8, ptr %t141, i64 -24
  %t143 = getelementptr inbounds { ptr, i64, i64 }, ptr %t142, i64 0, i32 0
  %t144 = getelementptr inbounds { ptr, i64, i64 }, ptr %t142, i64 0, i32 1
  %t145 = load ptr, ptr %t143
  %t146 = getelementptr inbounds { ptr, i64 }, ptr %t145, i64 %t128
  store { ptr, i64 } %t123, ptr %t146
  store i64 %t130, ptr %t144
  %t147 = load ptr, ptr %local.2
  ret ptr %t147
}

define internal i32 @fn.1() {
  %local.7 = alloca ptr
  %t17 = alloca { ptr, i64 }
  %t18 = alloca { ptr, i64 }
  %t28 = alloca { ptr, i64 }
  %t29 = alloca { ptr, i64 }
  %t0 = call ptr @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 11 }, { ptr, i64 } { ptr @.str.m0.1, i64 1 })
  store ptr %t0, ptr %local.7
  %t1 = load ptr, ptr %local.7
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
  %t8 = icmp ne i64 %t7, 2
  %t9 = icmp eq i1 %t8, 1
  br i1 %t9, label %on.body.4, label %on.end.3
on.body.4:
  ret i32 1
on.end.3:
  %t10 = load ptr, ptr %local.7
  %t11 = getelementptr inbounds i8, ptr %t10, i64 -24
  %t12 = getelementptr inbounds { ptr, i64, i64 }, ptr %t11, i64 0, i32 0
  %t13 = load ptr, ptr %t12
  %t14 = getelementptr inbounds { ptr, i64 }, ptr %t13, i32 0
  %t15 = load { ptr, i64 }, ptr %t14
  store { ptr, i64 } %t15, ptr %t17
  store { ptr, i64 } { ptr @.str.m0.2, i64 4 }, ptr %t18
  %t16 = call i1 @string_eq(ptr %t17, ptr %t18)
  %t19 = xor i1 %t16, 1
  %t20 = icmp eq i1 %t19, 1
  br i1 %t20, label %on.body.6, label %on.end.5
on.body.6:
  ret i32 2
on.end.5:
  %t21 = load ptr, ptr %local.7
  %t22 = getelementptr inbounds i8, ptr %t21, i64 -24
  %t23 = getelementptr inbounds { ptr, i64, i64 }, ptr %t22, i64 0, i32 0
  %t24 = load ptr, ptr %t23
  %t25 = getelementptr inbounds { ptr, i64 }, ptr %t24, i32 1
  %t26 = load { ptr, i64 }, ptr %t25
  store { ptr, i64 } %t26, ptr %t28
  store { ptr, i64 } { ptr @.str.m0.3, i64 6 }, ptr %t29
  %t27 = call i1 @string_eq(ptr %t28, ptr %t29)
  %t30 = xor i1 %t27, 1
  %t31 = icmp eq i1 %t30, 1
  br i1 %t31, label %on.body.8, label %on.end.7
on.body.8:
  ret i32 3
on.end.7:
  ret i32 0
}

@$split = internal alias ptr ({ ptr, i64 }, { ptr, i64 }), ptr @fn.0
@$main = alias i32 (), ptr @fn.1
