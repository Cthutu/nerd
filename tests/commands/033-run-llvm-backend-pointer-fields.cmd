use std.io

Point :: plex #c {
    x    i32
    y    i32
    name string
}

Packed :: plex #packed {
    a u8
    b i32
}

main :: fn () -> i32 {
    p := Point { x: 1, y: 2, name: "first" }
    q := p with { y: 7, name: "second" }
    pp := ^q
    prn($"{p.name} {p.x} {p.y}")
    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1, b: 2 }
    return q.x + q.y + packed.b
}
¬
10
¬
first 1 2
second 1 7

¬
delete
¬
--llvm-backend
