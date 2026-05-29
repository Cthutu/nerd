boxmod :: use test.imported_plex

Holder :: plex {
    box boxmod.Box
    ptr ^boxmod.Box
}

main :: fn () -> i32 {
    box : boxmod.Box
    box = boxmod.make_box(7)
    box.bump(1)
    holder := Holder { box, ptr: ^box }
    return (holder.box.value + holder.ptr^.value).as(i32)
}
¬
16
¬

¬
hir 0
module module.0(147-qualified-module-types.input)
import module.1(test.imported_plex)
import import.0 Box from module.1(test.imported_plex).decl.0: Box
import import.1 __impl_Box_bump from module.1(test.imported_plex).decl.1: fn (^Box^Box, usize) ->
import import.2 make_box from module.1(test.imported_plex).decl.2: fn (usize) -> Box
bind Box = import.0
bind __impl_Box_bump = import.1
bind make_box = import.2
bind boxmod = module.1
bind Holder = type.0
bind main = fn.0
bind Box = type.1
type type.0 = Holder
type type.1 = Box
func fn.0() -> i32 {
  expr <unknown> default
  let box: Box = <unknown> default
  assign Box local.0(box) = Box call fn (usize) -> Box field(module bind.3(boxmod), make_box)(usize 7)
  expr void call bind.1(__impl_Box_bump)(^Box address_of(Box local.0(box)), usize 1)
  let holder: Holder = Holder plex(box: Box local.0(box), ptr: ^Box address_of(Box local.0(box)))
  return i32 cast(usize add(usize field(Box field(Holder local.1(holder), box), value), usize field(Box deref(^Box field(Holder local.1(holder), ptr)), value)) as i32)
}
¬
; nerd llvm-ir 0
; generated from HIR

declare void @$__impl_Box_bump(ptr, i64)
declare { i64 } @$make_box(i64)

define internal i32 @fn.0() {
  %local.0 = alloca { i64 }
  store { i64 } zeroinitializer, ptr %local.0
  %t0 = call { i64 } @$make_box(i64 7)
  store { i64 } %t0, ptr %local.0
  call void @$__impl_Box_bump(ptr %local.0, i64 1)
  %t1 = load { i64 }, ptr %local.0
  %t2 = insertvalue { { i64 }, ptr } poison, { i64 } %t1, 0
  %t3 = insertvalue { { i64 }, ptr } %t2, ptr %local.0, 1
  %t4 = extractvalue { { i64 }, ptr } %t3, 0
  %t5 = extractvalue { i64 } %t4, 0
  %t6 = extractvalue { { i64 }, ptr } %t3, 1
  %t7 = load { i64 }, ptr %t6
  %t8 = extractvalue { i64 } %t7, 0
  %t9 = add i64 %t5, %t8
  %t10 = trunc i64 %t9 to i32
  ret i32 %t10
}

@$main = alias i32 (), ptr @fn.0
