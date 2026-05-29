print :: use core

main :: fn() {
    local_print :: use core

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
import module.1(core)
import import.0 pr from module.1(core).decl.11: fn (string) -> void
import import.1 prn from module.1(core).decl.13: fn (string) -> void
import import.2 __impl_arena_alloc from module.1(core).decl.16: <unknown>
import import.3 __impl_arena_alloc_array from module.1(core).decl.17: <unknown>
import import.4 __impl_arena_alloc_bytes from module.1(core).decl.18: fn (^arena^arena, usize) -
import import.5 __impl_arena_reset from module.1(core).decl.19: fn (^arena^arena) -
import import.6 __impl_arena_mark from module.1(core).decl.20: fn (^arena^arena)
import import.7 __impl_arena_restore from module.1(core).decl.21: fn (^arena^arena, u32) -
import import.8 __impl_arena_done from module.1(core).decl.22: fn (^arena^arena) -
bind pr = import.0
bind prn = import.1
bind __impl_arena_alloc = import.2
bind __impl_arena_alloc_array = import.3
bind __impl_arena_alloc_bytes = import.4
bind __impl_arena_reset = import.5
bind __impl_arena_mark = import.6
bind __impl_arena_restore = import.7
bind __impl_arena_done = import.8
bind print = module.1
bind main = fn.0
func fn.0() -> void {
  expr module <unsupported>
  let local_print: module = module <unsupported>
  expr void call fn (string) -> void field(module local.0(local_print), prn)(string "Local")
  expr void call fn (string) -> void field(module bind.9(print), prn)(string "Hello")
  expr void call fn (string) -> void field(module bind.9(print), pr)(string "same")
  expr void call fn (string) -> void field(module bind.9(print), prn)(string " line")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Local\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c"Hello\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"same\00"
@.str.m0.3 = private unnamed_addr constant [6 x i8] c" line\00"

declare void @$pr({ ptr, i64 })
declare void @$prn({ ptr, i64 })
declare { ptr, i64 } @$__impl_arena_alloc_bytes(ptr, i64)
declare void @$__impl_arena_reset(ptr)
declare i32 @$__impl_arena_mark(ptr)
declare void @$__impl_arena_restore(ptr, i32)
declare void @$__impl_arena_done(ptr)

define internal void @fn.0() {
  call void @$prn({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$pr({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  call void @$prn({ ptr, i64 } { ptr @.str.m0.3, i64 5 })
  ret void
}

@$main = alias void (), ptr @fn.0