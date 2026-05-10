Cell :: plex {
    values [4]i32
}

Box :: plex {
    cell Cell
}

take_cell :: fn (cell: Cell) -> i32 {
    return cell.values[1] + cell.values[2]
}

main :: fn () -> i32 {
    box: Box
    box.cell.values[1] = 7
    (^box.cell.values[2])^ = 9

    direct := take_cell(Cell { values: [1, 2, 3, 4] })
    return box.cell.values[1] + box.cell.values[2] + direct
}
¬
21
¬
¬
delete
¬
--llvm-backend
