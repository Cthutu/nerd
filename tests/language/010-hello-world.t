use std.io

-- Prints a simple string from a block-bodied main.
main :: fn () {
    prn("Hello, world!")
}
¬
0
¬
Hello, world!

¬
hir 0
module module.010-hello-world.input(010-hello-world.input)
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
  expr void call bind.2(prn_text)(string "Hello, world!")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [33 x i8] c"tests/language/010-hello-world.t\00"
@.str.m0.0 = private unnamed_addr constant [14 x i8] c"Hello, world!\00"

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.0, i64 13 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)