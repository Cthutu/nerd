use std.io
use test.generics
Alias :: u64
Item :: plex { number u64 }
Methods :: plex { marker i32 }
impl Methods {
    pair :: fn [T] (_self: Self, a: T, b: T) -> T { return a + b }
}
pair :: fn [T] (a: T, b: T) -> T { return a + b }
convert :: fn [T] (v: u32) -> T { return v.as() }
explicit :: fn [T] (v: u32) -> T { return v.as(T) }
return_cast :: fn (v: f64) -> u64 { return v.as() }
pointer :: fn (data: ^i32, log: ^i32) -> ^i32 { log^ = log^ * 10 + 1
    return data }
count :: fn (log: ^i32) -> usize { log^ = log^ * 10 + 2
    return 2 }
make :: fn (calls: ^i32) -> [2]i32 { calls^ += 1
    return [7, 8] }
main :: fn () {
    a: u32 = 42
    b: u64 = a.as()
    f: f64 = 3.1415
    assert return_cast(f.as()) == 3
    b = f.as()
    assert b == 3
    c: u64 = (a.as() + 1) * 2
    d: u8 = 256.as()
    wide: u64 = 4294967296.as()
    alias: Alias = a.as()
    Local :: u64
    named := a.as(Local)
    assert named == alias
    numbers: [2]u64 = [a.as(), f.as()]
    record: Item = { number: a.as() }
    tup: (u64, u8) = (a.as(), 256.as())
    assert numbers[1] == 3 && record.number == 42 && tup.0 == 42 && tup.1 == 0
    generic := pair(a.as(), alias)
    reversed := pair(alias, a.as())
    annotated: u64 = convert(a)
    assert annotated == 42 && explicit[u64](a) == 42
    methods: Methods
    assert methods.pair(a.as(), alias) == 84
    vals: [2]u64 = [9, 10]
    assert pick(vals.as(), alias) == 10
    assert pick(vals.as(), a.as(u64)) == 10
    assert pick(vals[..], a.as()) == 10
    assert pick([a.as(), f.as()], alias) == 3
    array := [1, 2, 3]
    view: []i32 = array.as()
    view[0] = 7
    n: usize = 2
    p := array.data
    short: []i32 = p.as(n)
    long: []i32 = p.as(n + 1)
    old := p.as([]i32, n)
    assert short.count == old.count && long.count == 3
    log: i32
    once: []i32 = pointer(p, ^log).as(count(^log))
    assert log == 12 && once[0] == 7
    header: Item = { number: 2 }
    field_count: []i32 = p.as(header.number.as(usize))
    assert field_count.count == 2
    calls: i32
    temporary: []i32 = make(^calls).as()
    assert calls == 1 && temporary[1] == 8
    length := 3
    runtime: [length]i32
    borrowed: []i32 = runtime.as()
    borrowed[1] = 99
    assert runtime[1] == 99 && borrowed.count == 3
    explicit_view := array.as([]i32)
    assert explicit_view.data == array.data
    address: usize = p.as()
    round_trip: ^i32 = address.as()
    assert round_trip == p
    text: string = "hello"
    bytes: []u8 = text.as()
    same: string = bytes.as()
    assert same == text
    prn($"casts {c} {d} {wide} {generic} {reversed}")
    prn($"views {view} {once.count} {borrowed} {log} {calls}")
}

¬
0
¬
casts 86 0 4294967296 84 84
views [7, 2, 3] 2 [0, 99, 0] 12 1

¬

¬

=== case: release-contextual-casts ===
-- test-release
main :: fn () {
    value: u32 = 42
    number: u64 = value.as() + 1
    buffer := [1, 2, 3]
    view: []i32 = buffer.as()
    view[..] = 7
    pointer := buffer.data
    count: usize = 2
    prefix: []i32 = pointer.as(count)
    assert prefix[1] == 7 && number == 43
    prn("release contextual casts")
}
¬
0
¬
release contextual casts
¬

¬
