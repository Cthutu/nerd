cells :: use test.qualified_generics
same_cells :: use test.qualified_generics
collections :: use test.generics

IntegerCell :: cells.Cell[i32]

Container :: plex {
    item cells.Cell[i64]
    nested cells.Cell[cells.Cell[i32]]
}

read :: fn (cell: ^cells.Cell[i32]) -> i32 { return cell.value }
pass :: fn [T] (value: T) -> cells.Cell[T] {
    return cells.wrap(value)
}

main :: fn () {
    cell: cells.Cell[i32]
    cell.value = 42
    assert read(^cell) == 42
    literal := cells.Cell[i32] { value: 43 }
    assert literal.get() == 43
    aliased: IntegerCell
    aliased.value = 44
    assert aliased.get() == 44
    same: same_cells.Cell[i32] = literal
    assert same.value == 43

    container: Container
    container.item.value = 5_000_000_000
    container.nested.value.value = 45
    assert container.item.get() == 5_000_000_000
    assert container.nested.value.get() == 45

    assert cells.identity(46) == 46
    assert cells.identity("qualified") == "qualified"
    wide: i64 = 5_000_000_000
    assert cells.identity(wide) == wide
    assert cells.identity[i64](5_000_000_001) == wide + 1
    assert cells.identity[i32](47) == 47
    identity := cells.identity[i32]
    assert identity(48) == 48

    constructed := cells.Cell[i64].init(5_000_000_002)
    assert constructed.get() == wide + 2
    wrapped := cells.wrap(49)
    assert wrapped.get() == 49
    forwarded := pass(50)
    assert forwarded.get() == 50

    box: collections.Box[i64]
    box.value = wide
    made := collections.make_box[i64](wide + 3)
    assert made.value == wide + 3
    assert collections.make_box(wide).value == box.value

    local_cells :: use test.qualified_generics
    assert local_cells.identity[i32](51) == 51
    local_cell: local_cells.Cell[i32]
    local_cell.value = 52
    assert local_cell.get() == 52

    -- Brackets on exported array values still index storage.
    index := 1
    assert cells.values[index] == 13
    cells.values[index] = 53
    assert same_cells.values[index] == 53
}
¬
0
¬
