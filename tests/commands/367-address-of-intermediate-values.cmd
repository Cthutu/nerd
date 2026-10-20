Point :: plex { x i32 y i32 }
constant :: 42
constant_point :: Point { x: 7, y: 9 }
constant_array :: [7, 9]

next :: fn (calls: ^i32) -> i32 { return calls^ += 1 }
make_point :: fn (calls: ^i32) -> Point {
    return Point { x: next(calls), y: 12 }
}
read :: fn (value: ^i32) -> i32 { return value^ }
identity :: fn [T](value: ^T) -> ^T { return value }

main :: fn () {
    a := 10
    b := 20
    sum := ^(a + b)
    assert sum^ == 30
    sum^ = 31
    assert a == 10
    assert b == 20
    assert sum^ == 31

    literal := ^42
    floating := ^1.5
    boolean := ^yes
    text := ^"hello"
    assert literal^ == 42
    assert floating^ == 1.5
    assert boolean^
    assert text^ == "hello"
    wide: ^i64 = ^5000000000
    narrow: ^u8 = ^42
    float32: ^f32 = ^1.5
    assert wide^ == 5000000000
    assert narrow^ == 42
    assert float32^ == 1.5
    assert read(^(a + b)) == 30
    assert identity[i32](^42)^ == 42

    calls := 0
    first := ^next(^calls)
    second := ^next(^calls)
    assert calls == 2
    assert first^ == 1
    assert second^ == 2
    assert first != second
    first^ = 100
    assert second^ == 2

    point := ^make_point(^calls)
    assert calls == 3
    assert point.x == 3
    point.y = 13
    assert point.y == 13
    storage := ^arena(4096)
    value := storage.alloc[i32]()
    value^ = 123
    assert value^ == 123
    storage.done()
    field := ^make_point(^calls).x
    assert calls == 4
    assert field^ == 4
    pair := ^(5, 6)
    assert pair^.0 == 5
    assert pair^.1 == 6
    selected := ^(on a == 10 => 8 else 9)
    blocked := ^${ break a + b }
    assert selected^ == 8
    assert blocked^ == 30

    local_constant :: 17
    local_copy := ^local_constant
    narrow_copy: ^u8 = ^local_constant
    grouped_copy := ^(local_constant)
    global_copy := ^constant
    wide_copy: ^i64 = ^constant
    global_copy_again := ^constant
    plex_copy := ^constant_point
    field_copy := ^constant_point.x
    element_copy := ^constant_array[0]
    local_copy^ = 18
    global_copy^ = 43
    plex_copy.x = 99
    field_copy^ = 100
    element_copy^ = 101
    assert local_constant == 17
    assert narrow_copy^ == 17
    assert grouped_copy^ == 17
    assert constant == 42
    assert wide_copy^ == 42
    assert global_copy_again^ == 42
    assert constant_point.x == 7
    assert constant_array[0] == 7
    assert element_copy^ == 101
    assert global_copy != global_copy_again
    assert field_copy^ == 100

    original := ^a
    original^ = 11
    assert a == 11
    defer {
        assert sum^ == 31
        assert point.y == 13
    }
    for i in [0..4] {
        temporary := ^(i + 10)
        assert temporary^ == i + 10
        defer { assert temporary^ == i + 10 }
    }
    -- Fixed temporary storage must be reused even without optimisation.
    for i in [0 .. 1_000_000] {
        temporary := ^(i + 1)
        assert temporary^ == i + 1
    }
}
¬
0
¬
