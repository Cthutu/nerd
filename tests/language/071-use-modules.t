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
module module.071-use-modules.input(071-use-modules.input)
import module.std.io(std.io)
import import.0 pr from module.core(core).decl.N: fn (string) -> void
import import.1 prn from module.core(core).decl.N: <unknown>
import import.2 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.3 prn_text from module.core(core).decl.N: fn (string) -> void
import import.4 prn_empty from module.core(core).decl.N: fn () -> void
bind pr = import.0
bind prn = import.1
bind input = import.2
bind prn_text = import.3
bind prn_empty = import.4
bind main = fn.0
func fn.0() -> void {
  expr void call bind.3(prn_text)(string "top")
  expr module <unsupported>
  let p: module = module <unsupported>
  expr void <unsupported>
  expr void call bind.3(prn_text)(string "local")
  expr void call bind.0(pr)(string "same")
  expr void call bind.3(prn_text)(string " line")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [33 x i8] c"tests/language/071-use-modules.t\00"
@.str.m0.0 = private unnamed_addr constant [4 x i8] c"top\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"local\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c" line\00"

declare void @$pr({ ptr, i64 })
declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 3 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$pr({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
