use test.lsp_types

Window :: Point

Panel :: plex {
    window Window
}

main :: fn () -> i32 {
    return 0
}
¬
0
¬

¬
hir 0
module module.0(181-bare-use-imported-types.input)
import module.1(test.lsp_types)
import import.0 Point from module.1(test.lsp_types).decl.0: Window
bind Point = import.0
bind Window = type.0
bind Panel = type.1
bind main = fn.0
type type.0 = Window
type type.1 = Panel
func fn.0() -> i32 {
  return i32 0
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  ret i32 0
}

@$main = alias i32 (), ptr @fn.0
