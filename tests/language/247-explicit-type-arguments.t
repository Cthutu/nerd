support :: use test.compiler_constructs
Pair :: plex [A, B] { first A
    second B }
Cell :: plex [T] { value T }
make_pair :: fn [A, B] (first: A, second: B) -> Pair[A, B] { return { first, second } }
Maker :: plex { }
impl Maker {
    pair :: fn [A, B] (self: ^Self, first: A, second: B) -> Pair[A, B] { return { first, second } }
}
main :: fn () -> i32 {
    pair := make_pair[i32, string](42, "answer")
    maker: Maker
    other := maker.pair[i32, string](7, "seven")
    cell: Cell[i32] = { value: 9 }
    nested := make_pair[Cell[i32], string](cell, "nested")
    values := [1, 2, 3]
    assert other.first == 7 && nested.first.value == 9 && values[1] == 2
    foreign := support.make_pair[i32, string](5, "five")
    assert foreign.first == 5
    prn(pair.second)
    return pair.first
}
¬
42
¬
answer
¬
¬
