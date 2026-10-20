support :: use test.compiler_constructs
Cell :: plex [T] { value T }
impl [T] Cell[T] { init :: fn (value: T) -> Self { return { value } } }
Pair :: plex [A, B] { first A
    second B }
impl [A, B] Pair[A, B] { init :: fn (first: A, second: B) -> Self { return { first, second } } }
main :: fn () -> i32 {
    cell := Cell[i32].init(42)
    pair := Pair[i32, string].init(9, "nine")
    nested := Cell[Cell[i32]].init(cell)
    assert pair.first == 9 && pair.second == "nine" && nested.value.value == 42
    foreign := support.Cell[i32].init(42)
    foreign_pair := support.Pair[i32, string].init(8, "eight")
    assert foreign.value == 42 && foreign_pair.first == 8 && foreign_pair.second == "eight"
    return cell.value
}
¬
42
¬
¬
¬
