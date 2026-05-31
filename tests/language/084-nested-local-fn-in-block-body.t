use std.io

lowercase_first :: fn (text: string) -> u8 {
    tolower :: fn (char: u8) -> u8 {
        return on char {
            'A' .. 'Z' => char + 32
            else => char
        }
    }

    chars := text
    for c in chars {
        return tolower(c^)
    }

    return 0
}

main :: fn () {
    prn($"{lowercase_first("ABC")}")
}
¬
0
¬
97

¬
hir 0
module module.0(084-nested-local-fn-in-block-body.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind lowercase_first = fn.0
bind main = fn.2
func fn.0(text: string, char: u8) -> u8 {
  let tolower: fn (u8) -> u8 = fn (u8) -> u8 fn.1
  let chars: string = string local.0(text)
  expr void for in c: ^u8 in string local.3(chars) {
    body {
      return u8 call local.1(tolower)(u8 deref(^u8 local.4(c)))
    }
  }
  return <unknown> 0
}
func fn.1(char: u8) -> u8 {
  return u8 on u8 local.2(char) {
    range_exclusive(u8 65, u8 90) => {
      expr u8 add(u8 local.2(char), u8 32)
    }
    else => {
      expr u8 local.2(char)
    }
  }
}
func fn.2() -> void {
  expr void call bind.0(prn)(string interpolate(u8 call bind.2(lowercase_first)(string "ABC")))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [51 x i8] c"tests/language/084-nested-local-fn-in-block-body.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"ABC\00"

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

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal i8 @fn.0({ ptr, i64 } %text, i8 %char) {
  %local.4 = alloca ptr
  %t0 = extractvalue { ptr, i64 } %text, 0
  %t1 = extractvalue { ptr, i64 } %text, 1
  %t2 = alloca i64
  store i64 0, ptr %t2
  br label %for.in.cond.0
for.in.cond.0:
  %t3 = load i64, ptr %t2
  %t4 = icmp ult i64 %t3, %t1
  br i1 %t4, label %for.in.body.1, label %for.in.end.3
for.in.body.1:
  %t5 = getelementptr inbounds i8, ptr %t0, i64 %t3
  store ptr %t5, ptr %local.4
  %t6 = load ptr, ptr %local.4
  %t7 = load i8, ptr %t6
  %t8 = call i8 @fn.1(i8 %t7)
  ret i8 %t8
for.in.update.2:
  %t9 = load i64, ptr %t2
  %t10 = add i64 %t9, 1
  store i64 %t10, ptr %t2
  br label %for.in.cond.0
for.in.end.3:
  ret i8 0
}

define internal i8 @fn.1(i8 %char) {
  %t0 = icmp sge i8 %char, 65
  %t1 = icmp slt i8 %char, 90
  %t2 = and i1 %t0, %t1
  br i1 %t2, label %on.body.1, label %on.next.2
on.body.1:
  %t3 = add i8 %char, 32
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t4 = phi i8 [%t3, %on.value.3], [%char, %on.value.6]
  ret i8 %t4
}

define internal void @fn.2() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call i8 @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  %t2 = alloca { ptr, i64 }
  call void @to_string$u8(ptr %t2, i8 zeroext %t1)
  call void @string_builder_append_string(ptr %t2)
  %t3 = alloca { ptr, i64 }
  call void @string_builder_finish(ptr %t3, i64 %t0)
  %t4 = load { ptr, i64 }, ptr %t3
  call void @$prn({ ptr, i64 } %t4)
  ret void
}

@$lowercase_first = internal alias i8 ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.2
