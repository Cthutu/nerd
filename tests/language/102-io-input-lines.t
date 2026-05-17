use std.io

main :: fn () {
    line := input("")
    prn(line)
}
¬
0
¬
hello world

¬
hir 0
module module.0(102-io-input-lines.input)
import module.1(std.io)
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.5: fn (string) -> string
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  let line: string = string call bind.1(input)(string "")
  expr void call bind.0(prn)(string local.0(line))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [1 x i8] c"\00"

declare void @$prn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal void @fn.0() {
  %t0 = call { ptr, i64 } @$input({ ptr, i64 } { ptr @.str.m0.0, i64 0 })
  call void @$prn({ ptr, i64 } %t0)
  ret void
}

@$main = alias void (), ptr @fn.0
¬
hello world