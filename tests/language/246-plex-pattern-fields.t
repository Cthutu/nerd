Point :: plex { x i32
 y i32
 name string }
Outer :: plex { before i32
 point Point
 after i32 }
Empty :: plex {}
Event :: enum { Moved(Point)
 Idle }
score :: fn (point: Point, wanted: i32) -> i32 {
    return on point {
        { x: for wanted, y: height, name: _ } => height
        { x: 0, ... } as matched => matched.y + 10
        { name: "special", ... } => 30
        { ... } => 40
        else => -1
    }
}
main :: fn () {
    assert score(Point { x: 2, y: 7, name: "a" }, 2) == 7
    assert score(Point { x: 0, y: 7, name: "a" }, 2) == 17
    assert score(Point { x: 1, y: 7, name: "special" }, 2) == 30
    assert score(Point { x: 1, y: 7, name: "a" }, 2) == 40
    point := Point { x: 2, y: 7, name: "a" }
    value := on point { { name: _, y, x: for (1 + 1) } => y
 else => 0 }
    assert value == 7
    nested := Outer { before: 1, point, after: 9 }
    result := on nested {
        { before: 1, point: Point { x: 2, ... }, after: last } => last
        else => 0
    }
    assert result == 9
    result2 := on nested {
        { point: { name: "a", x, y: _ }, ... } => x
        else => 0
    }
    assert result2 == 2
    event := Event.Moved(point)
    assert (on event { Moved { x: for value, ... } => 0
 Moved { y, ... } => y
 else => -1 }) == 7
    empty := Empty {}
    assert (on empty { {} => 1
 else => 0 }) == 1
    assert (on empty { { ... } => 2
 else => 0 }) == 2
}
¬
0
¬

¬

¬
