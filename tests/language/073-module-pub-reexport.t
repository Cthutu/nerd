wrapper :: use test.reexport

main :: fn() {
    wrapper.printer.prn("reexport")
    return 0
}
¬
0
¬
reexport

¬
hir 0
module module.0(073-module-pub-reexport.input)
import module.1(test.reexport)
import import.0 prn from module.2(core).decl.13: fn (string) -> void
import import.1 printer from module.1(test.reexport).decl.0: module
bind prn = import.0
bind printer = import.1
bind wrapper = module.1
bind main = fn.0
func fn.0() -> i32 {
  expr void call fn (string) -> void field(module field(module bind.2(wrapper), printer), prn)(string "reexport")
  return untyped integer 0
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [9 x i8] c"reexport\00"

declare void @$prn({ ptr, i64 })

define internal i32 @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 8 })
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
