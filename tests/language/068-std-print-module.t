print :: use std.io

main :: fn() {
    local_print :: use std.io

    local_print.prn("Local")
    print.prn("Hello")
    print.pr("same")
    print.prn(" line")
}
¬
0
¬
Local
Hello
same line

¬
hir 0
module module.0(068-std-print-module.input)
import module.1(std.io)
import import.0 prn from module.1(std.io).decl.8: fn (string) -> void
import import.1 pr from module.1(std.io).decl.6: fn (string) -> void
bind prn = import.0
bind pr = import.1
bind print = module.1
bind main = fn.0
func fn.0() -> void {
  expr module <unsupported>
  let local_print: module = module <unsupported>
  expr void call fn (string) -> void field(module local.0(local_print), prn)(string "Local")
  expr void call fn (string) -> void field(module bind.2(print), prn)(string "Hello")
  expr void call fn (string) -> void field(module bind.2(print), pr)(string "same")
  expr void call fn (string) -> void field(module bind.2(print), prn)(string " line")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Local\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"Hello\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c" line\00"

declare void @$prn({ ptr, i64 })
declare void @$pr({ ptr, i64 })

define internal void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$pr({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  ret void
}

@$main = alias void (), ptr @fn.0
