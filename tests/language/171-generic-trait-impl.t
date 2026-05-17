Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {
    show :: fn (self: Self) => $"Point({self.x}, {self.y})"
}

Box :: plex [T] {
    value T
}

impl Display for Box[T]
where T: Display {
    show :: fn (self: Self) -> string {
        return self.value.show()
    }
}

describe :: fn [T] (value: T) -> string
where T: Display {
    return value.show()
}

main :: fn () -> i32 {
    point := Point { x: 3, y: 4 }
    boxed: Box[Point]
    boxed.value = point
    return describe(boxed).count.as(i32) + boxed.show().count.as(i32)
}
¬
22
¬

¬
hir 0
¬
