calls: i32 = 0
Point :: plex { x i32
    y i32 }
make_point :: fn () -> Point { calls += 1
    return { x: 20 y: 22 } }
fetch :: fn (owner: ^Owned) -> ^Owned { calls += 1
    return owner }
Nested :: plex { point Point
    pair (i32, i32) }
Owned :: plex { data box[i32] }
main :: fn () -> i32 {
    point := make_point()
    { x, y } := point
    assert x + y == 42 && calls == 1
    { y: second, x: first } :: make_point()
    assert first + second == 42 && calls == 2
    on yes => { { x, y }: Point = point
        assert x + y == 42
    }
    { x, y } = Point { x: 21 y: 23 }
    assert x == 21 && y == 23
    nested := Nested { point: point pair: (1, 2) }
    { point: { x: xx }, pair: (a, b) } := nested
    assert xx == 20 && a == 1 && b == 2
    owner := Owned { data: box[i32]() }
    owner.data.data^ = 42
    { data } := owner
    assert owner.data == nil && data.data^ == 42
    data.free()
    owner.data = box[i32]()
    owner.data.data^ = 7
    { data: fetched } := fetch(^owner)^
    assert calls == 3 && fetched.data^ == 7 && owner.data == nil
    fetched.free()
    first_box := box[i32]()
    second_box := box[i32]()
    first_box.data^ = 1
    second_box.data^ = 2
    (first_box, second_box) = (second_box, first_box)
    assert first_box.data^ == 2 && second_box.data^ == 1
    (moved, _) := (first_box, 0)
    assert first_box == nil && moved.data^ == 2
    moved.free()
    second_box.free()
    return first + second
}
¬
42
¬
¬
¬
