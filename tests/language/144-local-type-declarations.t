main :: fn () -> i32 {
    Point :: plex {
        x i32
        y i32
    }

    Value :: union {
        i i32
        f f32
    }

    Choice :: enum { Left(i32) Right(i32) }

    point: Point = Point { x: 10, y: 20 }
    value: Value = Value { i: 7 }
    choice: Choice = Right(5)

    extra := on choice {
        Left(as n) => n
        Right(as n) => n
    }

    return point.x + point.y + value.i + extra
}
¬
42
¬

¬

¬
