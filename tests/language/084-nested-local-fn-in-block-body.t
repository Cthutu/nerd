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
import import.0 pr from module.1(std.io).decl.9: fn (string) -> void
import import.1 epr from module.1(std.io).decl.10: fn (string) -> void
import import.2 prn from module.1(std.io).decl.11: fn (string) -> void
import import.3 eprn from module.1(std.io).decl.12: fn (string) -> void
import import.4 input from module.1(std.io).decl.13: fn (string) -> string
bind pr = import.0
bind epr = import.1
bind prn = import.2
bind eprn = import.3
bind input = import.4
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
  expr void call bind.2(prn)(string interpolate(u8 call bind.5(lowercase_first)(string "ABC")))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"ABC\00"

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

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define i8 @fn.0({ ptr, i64 } %text, i8 %char) {
  %t0 = extractvalue { ptr, i64 } %text, 0
  %t1 = extractvalue { ptr, i64 } %text, 1
  %t2 = alloca i64
  store i64 0, ptr %t2
  %local.4 = alloca ptr
  br label %for.in.cond.0
for.in.cond.0:
  %t3 = load i64, ptr %t2
  %t4 = icmp ult i64 %t3, %t1
  br i1 %t4, label %for.in.body.1, label %for.in.end.2
for.in.body.1:
  %t5 = getelementptr inbounds i8, ptr %t0, i64 %t3
  store ptr %t5, ptr %local.4
  %t6 = load ptr, ptr %local.4
  %t7 = load i8, ptr %t6
  %t8 = call i8 @fn.1(i8 %t7)
  ret i8 %t8
for.in.end.2:
  ret i8 0
}

define i8 @fn.1(i8 %char) {
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

define void @fn.2() {
  %t0 = call i64 @string_builder_mark()
  %t1 = call i8 @fn.0({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  %t2 = call { ptr, i64 } @to_string$u8(i8 %t1)
  call void @string_builder_append_string({ ptr, i64 } %t2)
  %t3 = call { ptr, i64 } @string_builder_finish(i64 %t0)
  call void @$prn({ ptr, i64 } %t3)
  ret void
}

@$lowercase_first = alias i8 ({ ptr, i64 }), ptr @fn.0
@$main = alias void (), ptr @fn.2
