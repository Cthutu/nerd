-- Mutation still works for variables and storage reached through pointers/views.
support :: use tests.mods.immutable_aggregate_bindings
Point :: plex { x i32 }
Holder :: plex { point Point }
Value :: union { integer i32 real f32 }

change :: fn (point: ^Point) { point.x += 1 }
borrow :: fn (point: ^Point) -> ^Point { return point }
change_first :: fn (points: []Point) { points[0].x += 1 }

main :: fn () {
    point := Point { x: 20 }
    point.x += 1
    pointer :: borrow(^point)
    pointer.x += 1
    change(^point)
    assert point.x == 23

    holder := Holder { point: Point { x: 40 } }
    holder.point.x += 2
    assert holder.point.x == 42

    points := [Point { x: 20 }, Point { x: 22 }]
    points[0].x += 1
    view := points[..]
    borrowed :: view
    borrowed[0].x += 1
    change_first(view)
    assert points[0].x == 23

    value := Value { integer: 40 }
    value.integer += 2
    assert value.integer == 42

    original :: Point { x: 20 }
    copy := original
    copy.x = 42
    assert original.x == 20 && copy.x == 42

    support.variable.x = 42
    assert support.variable.x == 42
    prn("mutable aggregate storage passed")
}
¬
0
¬
mutable aggregate storage passed
¬
¬
