-- Content equality must honour each element's Eq behaviour and borrow owners.
Point :: plex {
    x i32
    ignored i32
}
comparisons := 0
impl [T] []T
where T: Eq {
    contains :: fn (s: ^Self, value: T) -> bool {
        for i in [0 .. s.count] {
            on s^[i] == value => return yes
        }
        return no
    }
}
same :: fn [T] (lhs: T, rhs: T) -> bool
where T: Eq { return lhs == rhs }
is_absent :: fn [T] (value: box[T]) => value == nil
main :: fn () {
    a := ["one", "two"]
    b := ["one", "two"]
    assert a == b && a[..] == b[..]
    assert same(a[..], b[..])
    assert a.contains("two")
    assert a[..1] != b[..]
    b[1] = "three"
    assert a != b && a[..] != b[..]
    empty: []string
    assert empty == a[..0]
    assert empty == nil
    assert a[..0] != nil

    f: [2]f64 = [0.0, 2.0]
    g: [2]f64 = [-0.0, 2.0]
    assert f == g && f[..] == g[..]
    single: [1]f32 = [0.0]
    negative: [1]f32 = [-0.0]
    assert single[..] == negative[..]
    zero: f64 = 0.0
    nan := zero / zero
    n := [nan]
    assert n[..] != n[..]
    assert n != n

    p := [Point {x: 1, ignored: 2}, Point {x: 2, ignored: 3}]
    q := [Point {x: 1, ignored: 8}, Point {x: 2, ignored: 9}]
    comparisons = 0
    assert p[..] == q[..]
    assert comparisons == 2
    assert same(p[..], q[..])
    assert p == q
    comparisons = 0
    assert p[..1] != q[..]
    assert comparisons == 0
    q[0].x = 10
    assert p[..] != q[..]
    assert comparisons == 1

    x := box[i32](2)
    y := box[i32](2)
    x.data[0] = 5
    x.data[1] = 0
    y.data[0] = 5
    y.data[1] = 0
    assert x.count == 2 && y.count == 2
    assert x.data != y.data
    assert x == y && !(x != y)
    assert x != nil && y != nil
    y.data[1] = 6
    assert x != y
    smaller := box[i32]()
    smaller.data[0] = 5
    assert x != smaller
    nil_box: box[i32]
    other_nil: box[i32]
    assert nil_box == other_nil
    assert nil_box != x && x != nil_box
    assert nil_box.count == 0

    z := box[Point]()
    w := box[Point]()
    z.x = 8
    w.x = 8
    z.ignored = 1
    w.ignored = 2
    assert z == w
    assert z.count == 1 && w.count == 1
    assert z.x == 8 && w.x == 8
    w.x = 9
    assert z != w

    nested_a := [a[..], b[..]]
    nested_b := [a[..], b[..]]
    assert nested_a[..] == nested_b[..]
    assert nested_a == nested_b
    nested_b[1] = a[..]
    assert nested_a[..] != nested_b[..]

    strings := box[string](2)
    other_strings := box[string](2)
    strings.data[0] = "hello"
    strings.data[1] = "world"
    other_strings.data[0] = "hello"
    other_strings.data[1] = "world"
    assert strings == other_strings
    assert strings.count == 2

    floats := box[f64]()
    other_floats := box[f64]()
    floats.data[0] = nan
    other_floats.data[0] = nan
    assert floats != other_floats && floats != floats
    floats.data[0] = 0.0
    other_floats.data[0] = -0.0
    assert floats == other_floats

    Strings :: []string
    boxed_slices := box[Strings]()
    other_boxed_slices := box[Strings]()
    boxed_slices.data[0] = a[..]
    other_boxed_slices.data[0] = a[..]
    assert boxed_slices == other_boxed_slices

    inner := box[i32]()
    other_inner := box[i32]()
    inner.data[0] = 42
    other_inner.data[0] = 42
    -- The slices borrow box storage; their comparisons must not consume it.
    box_view := (^inner).as([]box[i32], 1)
    other_box_view := (^other_inner).as([]box[i32], 1)
    assert box_view == other_box_view
    assert inner != nil && other_inner != nil
    assert inner.data[0] == 42
    arrays_a := [[1, 2], [3, 4]]
    arrays_b := [[1, 2], [3, 4]]
    assert arrays_a[..] == arrays_b[..]
    assert arrays_a == arrays_b
    assert is_absent[arena](nil)
    missing_arena: box[arena]
    assert missing_arena == nil
    Callback :: fn () -> i32
    missing_callback: box[Callback]
    assert missing_callback == nil
    prn("content equality passed")
}
impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) -> bool {
        comparisons += 1
        return lhs.x == rhs.x
    }
}
¬
0
¬
content equality passed
¬
delete
