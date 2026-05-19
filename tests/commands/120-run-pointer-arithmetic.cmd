Cell :: plex {
    value i32
}

main :: fn () -> i32 {
    values: [4]i32 = [10, 20, 30, 40]
    first := ^values[0]
    third := first + 2
    second := third - 1
    also_third := 2 + first
    distance := third - first

    cells: [2]Cell = [{ value : 7 }, { value : 11 }]
    cell_ptr := ^cells[0]
    next_cell := cell_ptr + 1
    previous_cell := next_cell - 1

    return third[0] + second[0] + also_third[0] + distance.as(i32) +
        next_cell.value + previous_cell.value
}
¬
100
¬

¬
delete
¬
