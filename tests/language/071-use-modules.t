use std.io

main :: fn() {
    prn("top")

    p :: use std.io
    use p

    prn("local")
    pr("same")
    prn(" line")
}
¬
0
¬
top
local
same line

¬
hir 0
module module.0(071-use-modules.input)
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
  expr void call bind.2(prn)(string "top")
  expr module <unsupported>
  let p: module = module <unsupported>
  expr void <unsupported>
  expr void call bind.2(prn)(string "local")
  expr void call bind.0(pr)(string "same")
  expr void call bind.2(prn)(string " line")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [4 x i8] c"top\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"local\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c" line\00"

declare void @$pr({ ptr, i64 })
declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$pr({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  ret void
}

@$main = alias void (), ptr @fn.0
