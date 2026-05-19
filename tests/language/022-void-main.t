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
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  expr void call bind.0(prn)(string "Hello from void main")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [53 x i8] c"tests/language/022-void-main.t\00"
@.str.m0.0 = private unnamed_addr constant [21 x i8] c"Hello from void main\00"

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 20 })
  ret void
}

@$main = alias void (), ptr @fn.0
