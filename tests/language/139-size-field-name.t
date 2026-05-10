Thing :: plex {
    size (i32, i32)
}

main :: fn () {
    thing: Thing
    thing.size = (7, 5)
    return thing.size.0 + thing.size.1
}
¬
12
¬

¬
hir 0
bind Thing = type.0
bind main = fn.0
type type.0 = Thing
func fn.0() -> i32 {
  expr <unknown> <unsupported>
  let thing: Thing = <unknown> <unsupported>
  assign (i32, i32) field(Thing local.0(thing), size) = (i32, i32) tuple(i32 7, i32 5)
  return i32 add(i32 tuple_field((i32, i32) field(Thing local.0(thing), size), 0), i32 tuple_field((i32, i32) field(Thing local.0(thing), size), 1))
}
¬
; nerd llvm-ir 0
; generated from HIR

define i32 @fn.0() {
  %t0 = insertvalue { i32, i32 } poison, i32 7, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 5, 1
  %local.0 = alloca { { i32, i32 } }
  store { { i32, i32 } } zeroinitializer, ptr %local.0
  %t2 = getelementptr inbounds { { i32, i32 } }, ptr %local.0, i64 0, i32 0
  store { i32, i32 } %t1, ptr %t2
  %t3 = load { { i32, i32 } }, ptr %local.0
  %t4 = extractvalue { { i32, i32 } } %t3, 0
  %t5 = extractvalue { i32, i32 } %t4, 0
  %t6 = load { { i32, i32 } }, ptr %local.0
  %t7 = extractvalue { { i32, i32 } } %t6, 0
  %t8 = extractvalue { i32, i32 } %t7, 1
  %t9 = add i32 %t5, %t8
  ret i32 %t9
}

@$main = alias i32 (), ptr @fn.0
