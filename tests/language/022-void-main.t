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
module module.022-void-main.input(022-void-main.input)
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
  expr void call bind.2(prn_text)(string "Hello from void main")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [31 x i8] c"tests/language/022-void-main.t\00"
@.str.m0.0 = private unnamed_addr constant [21 x i8] c"Hello from void main\00"

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 20 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)