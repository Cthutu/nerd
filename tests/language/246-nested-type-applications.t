support :: use test.compiler_constructs
Cell :: plex [T] { value T }
Nested :: Cell[Cell[i32]]
Deep :: Cell[Cell[Cell[i32]]]
Holder :: plex { value Cell[Cell[i32]] }
identity :: fn (value: Cell[Cell[i32]]) -> Cell[Cell[i32]] { return value }
main :: fn () -> i32 {
    nested: Nested = { value: { value: 42 } }
    deep: Deep = { value: { value: { value: 7 } } }
    holder: Holder = { value: nested }
    assert deep.value.value.value == 7
    foreign: support.Cell[support.Cell[i32]] = { value: { value: 42 } }
    assert foreign.value.value == 42
    return identity(holder.value).value.value
}
¬
42
¬
¬
¬
