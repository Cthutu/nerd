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
module module.133-std-io-error-output.input(133-std-io-error-output.input)
import module.std.io(std.io)
import import.0 epr from module.core(core).decl.N: <unknown>
import import.1 prn from module.core(core).decl.N: <unknown>
import import.2 eprn from module.core(core).decl.N: <unknown>
import import.3 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.4 epr_text from module.core(core).decl.N: fn (string) -> void
import import.5 epr_empty from module.core(core).decl.N: fn () -> void
import import.6 eprn_text from module.core(core).decl.N: fn (string) -> void
import import.7 eprn_empty from module.core(core).decl.N: fn () -> void
import import.8 prn_text from module.core(core).decl.N: fn (string) -> void
import import.9 prn_empty from module.core(core).decl.N: fn () -> void
bind epr = import.0
bind prn = import.1
bind eprn = import.2
bind input = import.3
bind epr_text = import.4
bind epr_empty = import.5
bind eprn_text = import.6
bind eprn_empty = import.7
bind prn_text = import.8
bind prn_empty = import.9
bind main = fn.0
func fn.0() -> void {
  expr void call bind.4(epr_text)(string "error")
  expr void call bind.6(eprn_text)(string " line")
  expr void call bind.8(prn_text)(string "done")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/133-std-io-error-output.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"error\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c" line\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"done\00"

declare ptr @$input({ ptr, i64 })
declare void @$epr_text({ ptr, i64 })
declare void @$epr_empty()
declare void @$eprn_text({ ptr, i64 })
declare void @$eprn_empty()
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$epr_text({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @$eprn_text({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)