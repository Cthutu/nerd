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
module module.0(010-hello-world.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  expr void call bind.0(prn)(string "Hello, world!")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [55 x i8] c"tests/language/010-hello-world.t\00"
@.str.m0.0 = private unnamed_addr constant [14 x i8] c"Hello, world!\00"

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 13 })
  ret void
}

@$main = alias void (), ptr @fn.0
