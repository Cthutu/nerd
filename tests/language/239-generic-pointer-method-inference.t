use std.io
Cell :: plex [T] { value T }
Reader :: plex { }
impl Reader {
    read :: fn [T] (self: ^Self, cell: ^Cell[T]) -> T { return cell.value }
    pair :: fn [T] (self: ^Self, fixed: i32, cell: ^Cell[T]) -> T {
        assert fixed == 7
        return cell.value
    }
}
main :: fn () -> i32 {
    reader : Reader
    integer : Cell[i32]
    decimal : Cell[f64]
    integer.value = 42
    decimal.value = 1.5
    assert reader.read(^integer) == 42
    assert reader.read(^decimal) == 1.5
    assert reader.pair(7, ^integer) == 42
    assert reader.read[i32](^integer) == 42
    prn("Generic pointer method inference passed")
    return 0
}
¬
0
¬
Generic pointer method inference passed

¬
hir 0
¬
; nerd llvm-ir 0
