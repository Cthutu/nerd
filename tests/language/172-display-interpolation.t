use std.io

Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) -> string {
        return $"Point({self.x}, {self.y})"
    }
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    text := $"Here: {point}"
    prn(text)
    return text.count.as(i32)
}
¬
17
¬
Here: Point(3, 4)

¬
hir 0
¬
