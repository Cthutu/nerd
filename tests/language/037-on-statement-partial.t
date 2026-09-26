use std.io

-- Statement-position block-form `on` may omit `else`.
main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }

    prn("done")
}
¬
0
¬
done

¬
hir 0
module module.037-on-statement-partial.input(037-on-statement-partial.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> void {
  let value: untyped integer = untyped integer 2
  expr void on i32 local.0(value) {
    value(i32 1) => {
      expr void call bind.2(prn_text)(string "one")
    }
  }
  expr void call bind.2(prn_text)(string "done")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [42 x i8] c"tests/language/037-on-statement-partial.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"one\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"done\00"

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  %t0 = icmp eq i32 2, 1
  br i1 %t0, label %on.body.1, label %on.end.0
on.body.1:
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  br label %on.end.0
on.end.0:
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)