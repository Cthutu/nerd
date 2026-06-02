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
  %t42 = alloca i1
  br i1 %t41, label %logical.rhs.14, label %logical.short.15
logical.short.15:
  store i1 0, ptr %t42
  br label %logical.end.16
logical.rhs.14:
  %t44 = load i64, ptr %local.6
  %t45 = extractvalue { ptr, i64 } %sep, 1
  %t46 = icmp ult i64 %t44, %t45
  store i1 %t46, ptr %t42
  br label %logical.end.16
logical.end.16:
  %t43 = load i1, ptr %t42
  br i1 %t43, label %for.body.11, label %for.end.13
for.body.11:
  %t47 = extractvalue { ptr, i64 } %s, 0
  %t48 = load i64, ptr %local.4
  %t49 = load i64, ptr %local.6
  %t50 = add i64 %t48, %t49
  %t51 = getelementptr inbounds i8, ptr %t47, i64 %t50
  %t52 = load i8, ptr %t51
  %t53 = extractvalue { ptr, i64 } %sep, 0
  %t54 = load i64, ptr %local.6
  %t55 = getelementptr inbounds i8, ptr %t53, i64 %t54
  %t56 = load i8, ptr %t55
  %t57 = icmp ne i8 %t52, %t56
  %t58 = icmp eq i1 %t57, 1
  br i1 %t58, label %on.body.18, label %on.end.17
on.body.18:
  store i1 0, ptr %local.5
  br label %on.end.17
on.end.17:
  br label %for.update.12
for.update.12:
  %t59 = load i64, ptr %local.6
  %t60 = add i64 %t59, 1
  store i64 %t60, ptr %local.6
  br label %for.cond.10
for.end.13:
  %t61 = load i1, ptr %local.5
  %t62 = icmp eq i1 %t61, 1
  br i1 %t62, label %on.body.20, label %on.next.21
on.body.20:
  %t63 = load ptr, ptr %local.2
  %t64 = icmp eq ptr %t63, null
  br i1 %t64, label %dynarray.alloc.22, label %dynarray.ready.23
dynarray.alloc.22:
  %t65 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 18)
  %t66 = getelementptr inbounds { ptr, i64, i64 }, ptr %t65, i64 0, i32 0
  %t67 = getelementptr inbounds { ptr, i64, i64 }, ptr %t65, i64 0, i32 1
  %t68 = getelementptr inbounds { ptr, i64, i64 }, ptr %t65, i64 0, i32 2
  %t69 = getelementptr inbounds i8, ptr %t65, i64 24
  store ptr %t69, ptr %t66
  store i64 0, ptr %t67
  store i64 0, ptr %t68
  store ptr %t69, ptr %local.2
  br label %dynarray.ready.23
dynarray.ready.23:
  %t70 = load ptr, ptr %local.2
  %t71 = getelementptr inbounds i8, ptr %t70, i64 -24
  %t72 = extractvalue { ptr, i64 } %s, 0
  %t73 = extractvalue { ptr, i64 } %s, 1
  %t74 = load i64, ptr %local.3
  %t75 = load i64, ptr %local.4
  %t76 = sub i64 %t75, %t74
  %t77 = getelementptr inbounds i8, ptr %t72, i64 %t74
  %t78 = insertvalue { ptr, i64 } poison, ptr %t77, 0
  %t79 = insertvalue { ptr, i64 } %t78, i64 %t76, 1
  %t80 = getelementptr inbounds { ptr, i64, i64 }, ptr %t71, i64 0, i32 0
  %t81 = getelementptr inbounds { ptr, i64, i64 }, ptr %t71, i64 0, i32 1
  %t82 = getelementptr inbounds { ptr, i64, i64 }, ptr %t71, i64 0, i32 2
  %t83 = load ptr, ptr %t80
  %t84 = load i64, ptr %t81
  %t85 = load i64, ptr %t82
  %t86 = add i64 %t84, 1
  %t87 = icmp ugt i64 %t86, %t85
  br i1 %t87, label %dynarray.grow.24, label %dynarray.store.25
dynarray.grow.24:
  %t88 = icmp eq i64 %t85, 0
  %t89 = mul i64 %t85, 2
  %t90 = select i1 %t88, i64 1, i64 %t89
  %t91 = mul i64 %t90, 16
  %t92 = add i64 24, %t91
  %t93 = call ptr @nrt_mem_realloc(ptr %t71, i64 %t92, i64 16, ptr @.macro.file.m0, i32 18)
  %t94 = getelementptr inbounds i8, ptr %t93, i64 24
  %t95 = getelementptr inbounds { ptr, i64, i64 }, ptr %t93, i64 0, i32 0
  %t96 = getelementptr inbounds { ptr, i64, i64 }, ptr %t93, i64 0, i32 2
  store ptr %t94, ptr %t95
  store i64 %t90, ptr %t96
  store ptr %t94, ptr %local.2
  br label %dynarray.store.25
dynarray.store.25:
  %t97 = load ptr, ptr %local.2
  %t98 = getelementptr inbounds i8, ptr %t97, i64 -24
  %t99 = getelementptr inbounds { ptr, i64, i64 }, ptr %t98, i64 0, i32 0
  %t100 = getelementptr inbounds { ptr, i64, i64 }, ptr %t98, i64 0, i32 1
  %t101 = load ptr, ptr %t99
  %t102 = getelementptr inbounds { ptr, i64 }, ptr %t101, i64 %t84
  store { ptr, i64 } %t79, ptr %t102
  store i64 %t86, ptr %t100
  %t103 = load i64, ptr %local.4
  %t104 = extractvalue { ptr, i64 } %sep, 1
  %t105 = add i64 %t103, %t104
  store i64 %t105, ptr %local.4
  %t106 = load i64, ptr %local.4
  store i64 %t106, ptr %local.3
  br label %on.end.19
on.next.21:
  br label %on.body.26
on.body.26:
  %t107 = load i64, ptr %local.4
  %t108 = add i64 %t107, 1
  store i64 %t108, ptr %local.4
  br label %on.end.19
on.end.19:
  br label %for.cond.6
for.end.9:
  %t109 = load ptr, ptr %local.2
  %t110 = icmp eq ptr %t109, null
  br i1 %t110, label %dynarray.alloc.27, label %dynarray.ready.28
dynarray.alloc.27:
  %t111 = call ptr @nrt_mem_alloc(i64 24, i64 16, ptr @.macro.file.m0, i32 26)
  %t112 = getelementptr inbounds { ptr, i64, i64 }, ptr %t111, i64 0, i32 0
  %t113 = getelementptr inbounds { ptr, i64, i64 }, ptr %t111, i64 0, i32 1
  %t114 = getelementptr inbounds { ptr, i64, i64 }, ptr %t111, i64 0, i32 2
  %t115 = getelementptr inbounds i8, ptr %t111, i64 24
  store ptr %t115, ptr %t112
  store i64 0, ptr %t113
  store i64 0, ptr %t114
  store ptr %t115, ptr %local.2
  br label %dynarray.ready.28
dynarray.ready.28:
  %t116 = load ptr, ptr %local.2
  %t117 = getelementptr inbounds i8, ptr %t116, i64 -24
  %t118 = extractvalue { ptr, i64 } %s, 0
  %t119 = extractvalue { ptr, i64 } %s, 1
  %t120 = load i64, ptr %local.3
  %t121 = sub i64 %t119, %t120
  %t122 = getelementptr inbounds i8, ptr %t118, i64 %t120
  %t123 = insertvalue { ptr, i64 } poison, ptr %t122, 0
  %t124 = insertvalue { ptr, i64 } %t123, i64 %t121, 1
  %t125 = getelementptr inbounds { ptr, i64, i64 }, ptr %t117, i64 0, i32 0
  %t126 = getelementptr inbounds { ptr, i64, i64 }, ptr %t117, i64 0, i32 1
  %t127 = getelementptr inbounds { ptr, i64, i64 }, ptr %t117, i64 0, i32 2
  %t128 = load ptr, ptr %t125
  %t129 = load i64, ptr %t126
  %t130 = load i64, ptr %t127
  %t131 = add i64 %t129, 1
  %t132 = icmp ugt i64 %t131, %t130
  br i1 %t132, label %dynarray.grow.29, label %dynarray.store.30
dynarray.grow.29:
  %t133 = icmp eq i64 %t130, 0
  %t134 = mul i64 %t130, 2
  %t135 = select i1 %t133, i64 1, i64 %t134
  %t136 = mul i64 %t135, 16
  %t137 = add i64 24, %t136
  %t138 = call ptr @nrt_mem_realloc(ptr %t117, i64 %t137, i64 16, ptr @.macro.file.m0, i32 26)
  %t139 = getelementptr inbounds i8, ptr %t138, i64 24
  %t140 = getelementptr inbounds { ptr, i64, i64 }, ptr %t138, i64 0, i32 0
  %t141 = getelementptr inbounds { ptr, i64, i64 }, ptr %t138, i64 0, i32 2
  store ptr %t139, ptr %t140
  store i64 %t135, ptr %t141
  store ptr %t139, ptr %local.2
  br label %dynarray.store.30
dynarray.store.30:
  %t142 = load ptr, ptr %local.2
  %t143 = getelementptr inbounds i8, ptr %t142, i64 -24
  %t144 = getelementptr inbounds { ptr, i64, i64 }, ptr %t143, i64 0, i32 0
  %t145 = getelementptr inbounds { ptr, i64, i64 }, ptr %t143, i64 0, i32 1
  %t146 = load ptr, ptr %t144
  %t147 = getelementptr inbounds { ptr, i64 }, ptr %t146, i64 %t129
  store { ptr, i64 } %t124, ptr %t147
  store i64 %t131, ptr %t145
  %t148 = load ptr, ptr %local.2
  ret ptr %t148
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

