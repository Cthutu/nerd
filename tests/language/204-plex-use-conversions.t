imported :: use test.imported_plex
Point :: plex { x f32 y f32 }
Rect :: plex { prefix u8 use Point w f32 h f32 }
Layer :: plex { before u64 use Rect }
Outer :: plex { before_outer u8 use Layer }
Top :: plex { tag u64 use Outer }
Pair :: plex [T] { first T second T }
IntPair :: plex { prefix u8 use Pair[i32] }
Bits :: plex { u8 { small: 3 large: 5 } }
BitContainer :: plex { prefix u64 use Bits tail u8 }
External :: plex { prefix u8 use imported.Box }
initial_rect :: Rect { prefix: 1 x: 2.0 y: 3.0 w: 4.0 h: 5.0 }
initial_point: Point = initial_rect
calls : i32
make_rect :: fn () -> Rect {
    calls += 1
    return Rect { prefix: 7 x: 10.0 y: 20.0 w: 30.0 h: 40.0 }
}
point_value :: fn (r: Rect) -> Point { return r }
point_pointer :: fn (r: ^Rect) -> ^Point { return r }
accept_point :: fn (p: Point) -> f32 { return p.x + p.y }
move_point :: fn (p: ^Point) { p.x += 1.0 p.y += 2.0 }
base_choice :: fn (_p: ^Point) -> i32 { return 1 }
rect_choice :: fn (_r: ^Rect) -> i32 { return 2 }
choice :: fn { base_choice rect_choice }
choice_reverse :: fn { rect_choice base_choice }
wrapped_choice :: fn (_p: ?Point) -> i32 { return 1 }
value_choice :: fn (_r: Rect) -> i32 { return 2 }
choice_wrapped :: fn { wrapped_choice value_choice }
optional_value :: fn (r: Rect) -> ?Point { return r }
result_value :: fn (r: Rect) -> Point\i32 { return r }
main :: fn () -> i32 {
    assert initial_point.x == 2.0 && initial_point.y == 3.0
    r := make_rect()
    p: Point = r
    p.x = 99.0
    assert r.x == 10.0
    assert accept_point(r) == 30.0
    p = r
    assert p.x == 10.0 && p.y == 20.0
    pp: ^Point = ^r
    move_point(pp)
    move_point(^r)
    pp = ^r
    assert r.x == 12.0 && r.y == 24.0 && r.prefix == 7 && r.w == 30.0
    assert point_value(r).x == 12.0
    assert point_pointer(^r) == pp
    assert r.as(Point).y == 24.0
    assert (^r).as(^Point) == pp
    assert choice(^r) == 2 && choice_reverse(^r) == 2
    assert choice(pp) == 1
    assert choice_wrapped(r) == 2
    optional_copy := optional_value(r) else { return 8 }
    result_copy := result_value(r) else { return 9 }
    assert optional_copy.x == 12.0 && result_copy.y == 24.0
    copy: Point = make_rect()
    assert calls == 2 && copy.x == 10.0
    top := Top { tag: 1 before_outer: 2 before: 3 prefix: 4
                 x: 5.0 y: 6.0 w: 7.0 h: 8.0 }
    move_point(^top)
    from_top: Point = top
    assert from_top.x == 6.0 && from_top.y == 8.0
    assert (^top).as(^Point).x == 6.0
    null_rect: ^Rect = nil
    null_point: ^Point = null_rect
    assert null_point == nil && null_rect.as(^Point) == nil
    pair := IntPair { prefix: 3 first: 4 second: 5 }
    base_pair: Pair[i32] = pair
    base_ptr: ^Pair[i32] = ^pair
    base_ptr.first = 6
    assert base_pair.first == 4 && pair.first == 6
    bit_container := BitContainer { prefix: 7 small: 3 large: 12 tail: 9 }
    bits: Bits = bit_container
    bit_ptr: ^Bits = ^bit_container
    bit_ptr.small = 4
    assert bits.small == 3 && bit_container.small == 4 && bit_container.tail == 9
    ext := External { prefix: 1 value: 23 }
    box: imported.Box = ext
    box_ptr: ^imported.Box = ^ext
    box_ptr.value = 24
    assert box.value == 23 && ext.value == 24
    indirect: fn (p: ^Point) = move_point
    indirect(^r)
    assert r.x == 13.0
    return 0
}
¬
0
¬

¬

¬
