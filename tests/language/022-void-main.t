use std.io

-- Allows a block-bodied main with no explicit return value.
main :: fn () {
    prn("Hello from void main")
}
¬
0
¬
Hello from void main

¬
hir 0
module module.0(022-void-main.input)
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
  expr void call bind.2(prn)(string "Hello from void main")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [21 x i8] c"Hello from void main\00"

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 20 })
  ret void
}

@$main = alias void (), ptr @fn.0
