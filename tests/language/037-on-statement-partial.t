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
module module.0(037-on-statement-partial.input)
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
bind main = fn.0
func fn.0() -> void {
  let value: untyped integer = untyped integer 2
  expr void on i32 local.0(value) {
    value(i32 1) => {
      expr void call bind.2(prn)(string "one")
    }
  }
  expr void call bind.2(prn)(string "done")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"one\00"
@.str.m0.1 = private unnamed_addr constant [5 x i8] c"done\00"

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define void @fn.0() {
  %t0 = icmp eq i32 2, 1
  br i1 %t0, label %on.body.1, label %on.end.0
on.body.1:
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  br label %on.end.0
on.end.0:
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 4 })
  ret void
}

@$main = alias void (), ptr @fn.0
