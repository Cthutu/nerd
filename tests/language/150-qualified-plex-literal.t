types :: use test.lsp_types

main :: fn () -> i32 {
    point := types.Point {
        x: 3
        y: 4
    }
    return point.x + point.y
}
¬
7
¬

¬
hir 0
module module.0(150-qualified-plex-literal.input)
import module.1(test.lsp_types)
import import.0 Point from module.1(test.lsp_types).decl.0: Point
bind Point = import.0
bind types = module.1
bind main = fn.0
func fn.0() -> i32 {
  let point: Point = Point plex(x: i32 3, y: i32 4)
  return i32 add(i32 field(Point local.0(point), x), i32 field(Point local.0(point), y))
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal i32 @fn.0() {
  %t0 = insertvalue { i32, i32 } poison, i32 3, 0
  %t1 = insertvalue { i32, i32 } %t0, i32 4, 1
  %t2 = extractvalue { i32, i32 } %t1, 0
  %t3 = extractvalue { i32, i32 } %t1, 1
  %t4 = add i32 %t2, %t3
  ret i32 %t4
}

@$main = alias i32 (), ptr @fn.0
