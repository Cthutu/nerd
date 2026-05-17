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
import import.1 __impl_132_alloc from module.1(test.reexport).decl.3: <unknown>
import import.2 __impl_132_alloc_array from module.1(test.reexport).decl.4: <unknown>
import import.3 __impl_132_alloc_bytes from module.1(test.reexport).decl.5: fn (^arena^arena, usize) -
import import.4 __impl_132_reset from module.1(test.reexport).decl.6: fn (^arena^arena) -
import import.5 __impl_132_mark from module.1(test.reexport).decl.7: fn (^arena^arena)
import import.6 __impl_132_restore from module.1(test.reexport).decl.8: fn (^arena^arena, u32) -
import import.7 __impl_132_done from module.1(test.reexport).decl.9: fn (^arena^arena) -
import import.8 printer from module.1(test.reexport).decl.0: module
bind prn = import.0
bind __impl_132_alloc = import.1
bind __impl_132_alloc_array = import.2
bind __impl_132_alloc_bytes = import.3
bind __impl_132_reset = import.4
bind __impl_132_mark = import.5
bind __impl_132_restore = import.6
bind __impl_132_done = import.7
bind printer = import.8
bind wrapper = module.1
bind main = fn.0
func fn.0() -> i32 {
  expr void call fn (string) -> void field(module field(module bind.9(wrapper), printer), prn)(string "reexport")
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