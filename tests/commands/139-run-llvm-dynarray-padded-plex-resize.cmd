Tile :: enum { Wall Floor }

Cell :: plex {
    tile   Tile
    region i32
}

cells : [..]Cell

main :: fn () -> i32 {
    cells.resize_to(697)
    on cells.count != 697 => return 1

    cells[696] = Cell { tile: Tile.Floor region: 42 }
    on cells[696].tile != Tile.Floor => return 2
    on cells[696].region != 42 => return 3

    cells.free()
    return 0
}
¬
0
¬

¬
delete
¬
