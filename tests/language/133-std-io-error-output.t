use std.io

main :: fn () {
    epr("error")
    eprn(" line")
    prn("done")
}
¬
0
¬
done

¬
hir 0
module module.0(133-std-io-error-output.input)
import module.1(std.io)
import import.0 epr from module.2(core).decl.12: fn (string) -> void
import import.1 prn from module.2(core).decl.13: fn (string) -> void
import import.2 eprn from module.2(core).decl.14: fn (string) -> void
import import.3 input from module.1(std.io).decl.5: fn (string) -> string
bind epr = import.0
bind prn = import.1
bind eprn = import.2
bind input = import.3
bind main = fn.0
func fn.0() -> void {
  expr void call bind.0(epr)(string "error")
  expr void call bind.2(eprn)(string " line")
  expr void call bind.1(prn)(string "done")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"error\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c" line\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"done\00"

declare void @$epr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare void @$eprn({ ptr, i64 })
declare { ptr, i64 } @$input({ ptr, i64 })

define internal void @fn.0() {
  call void @$epr({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @$eprn({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  ret void
}

@$main = alias void (), ptr @fn.0