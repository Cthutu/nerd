io :: use std.io

Box :: plex [T] {
    value T
}

Cell :: union [T] {
    number T
    text   string
}

Maybe :: enum [T] { None Some(T) }

main :: fn () {
    b: Box[i32]
    b.value = 7

    c: Cell[i32]
    c.number = b.value + 1

    m: Maybe[i32] = Some(c.number)
    on m {
        Some(as value) => io.prn($"generic {value}")
        None => io.prn("none")
    }
}
¬
0
¬
generic 8

¬

¬
