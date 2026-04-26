use std.print

Point :: plex { x i32 y i32 }

origin : Point : { x: 9, y: 8 }
points : []Point : [
    { x: 1, y: 2 },
    { x: 3, y: 4 },
]

show_last :: fn (items: []Point) {
    last :: items[items.count - 1]
    prn($"{last.x} {last.y}")
}

main :: fn () {
    p : Point = { x: 5, y: 6 }
    prn($"{origin.x} {origin.y}")
    prn($"{points[0].x} {points[1].y}")
    prn($"{p.x} {p.y}")
    show_last([
        { x: 13, y: 14 },
        { x: 15, y: 16 },
    ])
}
¬0¬9 8
1 4
5 6
15 16

¬¬
