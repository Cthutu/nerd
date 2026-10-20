describe :: fn [T] (value: T) -> string
where T: Display { return value.show() }
main :: fn () {
    assert describe(42) == "42"
    assert describe(yes) == "yes"
    assert describe("text") == "text"
    assert describe(1.5) == $"{1.5}"
    values := [1, 2]
    assert describe(values) == $"{values}"
    view: []i32 = values
    assert describe(view) == $"{view}"
    pair := (1, "two")
    assert describe(pair) == $"{pair}"
    dynamic: [..]i32
    dynamic.push(4)
    assert describe(dynamic) == $"{dynamic}"
    dynamic.free()
    assert describe(42.as(i8)) == "42" && describe(42.as(i16)) == "42"
    assert describe(42.as(i64)) == "42" && describe(42.as(isize)) == "42"
    assert describe(42.as(u8)) == "42" && describe(42.as(u16)) == "42"
    assert describe(42.as(u32)) == "42" && describe(42.as(u64)) == "42"
    assert describe(42.as(usize)) == "42"
    assert describe(1.5.as(f32)) == $"{1.5.as(f32)}"
    prn(describe(42))
}
¬
0
¬
42
¬
¬
