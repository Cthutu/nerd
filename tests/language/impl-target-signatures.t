use test.impl_target_types
imported :: use test.impl_target_types

Thing :: plex { value i32 }
Alias :: Thing
stored: Thing = { value: 99 }

impl Thing {
    make :: fn (value: i32) -> Thing { return { value } }
    make_alias :: fn (value: i32) -> Alias { return { value } }
    maybe :: fn (value: i32) -> ?Thing { return Thing { value } }
    pair :: fn (value: i32) -> (Thing, i32) { return (Thing { value }, value) }
    pointer :: fn () -> ^Thing { return ^stored }
    get :: fn (self: Thing) -> i32 { return self.value }
    put :: fn (self: ^Thing, value: i32) { self.value = value }
    alias_get :: fn (self: Alias) -> i32 { return self.value }
    generic_get :: fn [U] (self: Alias, _other: U) -> i32 { return self.value }
    generic_make :: fn [U] (value: i32, _other: U) -> Alias { return { value } }
    generic_pair :: fn [U] (other: U) -> (Thing, U) { return (Thing { value: 2 }, other) }
    generic_cast :: fn [U] (_first: U, _second: U) -> Thing { return { value: 3 } }
}
impl Alias {
    from_alias :: fn (value: i32) -> Thing { return { value } }
}
impl imported.Shape {
    extra :: fn (value: i32) -> imported.Shape { return { value } }
}
Defaults :: plex { value i32 }
impl Default for Defaults {
    default :: fn () -> Defaults { return { value: 5 } }
}

main :: fn () {
    thing := Thing.make(1)
    thing.put(42)
    assert thing.get() == 42 && thing.alias_get() == 42
    assert thing.generic_get("unused") == 42
    assert Thing.generic_make(3, "unused").value == 3
    generic_pair := Thing.generic_pair("payload")
    assert generic_pair.0.value == 2 && generic_pair.1 == "payload"
    wide: u64 = 9
    assert Thing.generic_cast(7.as(), wide).value == 3
    assert Thing.make_alias(4).value == 4 && Alias.from_alias(6).value == 6
    assert Thing.pointer().value == 99
    pair := Thing.pair(8)
    assert pair.0.value == 8 && pair.1 == 8
    optional := on Thing.maybe(9) { value => value else => Thing { value: 0 } }
    assert optional.value == 9
    shape := ShapeAlias.make(10)
    shape.put(11)
    assert shape.get() == 11
    assert imported.Shape.extra(12).value == 12
    assert imported.Shape.generic_make("unused").value == 14
    box := imported.Box[i32].make(13)
    assert box.get() == 13
    defaults: Defaults
    assert defaults.value == 5
    prn("impl target signatures passed")
}
¬
0
¬
impl target signatures passed
¬

¬
