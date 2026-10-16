use std.io
Compact :: plex { first u8 middle u32 last u8 }
COrder :: plex #c { first u8 middle u32 last u8 }
Generic :: plex [T] { first u8 middle T last u8 }
Embedded :: plex { head u8 use Compact tail u8 }
CEmbedded :: plex #c { head u8 use Compact tail u8 }
Kind :: enum { First Second }
TaggedBits :: plex { before u8 u32 { kind Kind : 8 payload : 24 } after u8 }
UseTagged :: plex { tag u64 use TaggedBits }
Nested :: plex { before u8 use Embedded after u64 }
Blocks :: plex { first u8 u32 { low : 8 high : 24 } last u8 }
constant :: Compact { first: 1 middle: 200 last: 3 }
global := Compact { first: 4 middle: 500 last: 6 }
calls := 0
next :: fn () -> u32 { calls += 1 return calls.as(u32) }
copy :: fn (value: Compact) -> Compact { return value }
main :: fn () {
    assert Compact.size == 8
    assert COrder.size == 12
    assert Embedded.size == 12
    assert Nested.size == 24
    assert Blocks.size == 8
    assert TaggedBits.size == 8 && UseTagged.size == 16
    assert CEmbedded.size == 16
    value := Compact { first: next().as(u8) middle: next() last: next().as(u8) }
    assert calls == 3 && value.first == 1 && value.middle == 2 && value.last == 3
    assert (^value.middle).as(usize) - (^value).as(usize) == 0
    assert (^value.first).as(usize) - (^value).as(usize) == 4
    assert (^value.last).as(usize) - (^value).as(usize) == 5
    c := COrder { first: 7 middle: 800 last: 9 }
    assert (^c.first).as(usize) - (^c).as(usize) == 0
    assert (^c.middle).as(usize) - (^c).as(usize) == 4
    assert (^c.last).as(usize) - (^c).as(usize) == 8
    assert constant.first == 1 && constant.middle == 200 && constant.last == 3
    assert global.first == 4 && global.middle == 500 && global.last == 6
    global.middle = 501
    assert copy(global).middle == 501
    changed := value with { middle: 42 }
    assert changed.first == 1 && changed.middle == 42 && changed.last == 3
    values: [2]Compact = [value, changed]
    assert values.size == 16 && values[1].middle == 42
    assert (^values[1]).as(usize) - (^values[0]).as(usize) == 8
    dynamic: [..]Compact
    dynamic.push(value)
    dynamic.push(changed)
    assert dynamic[0].first == 1 && dynamic[1].middle == 42
    dynamic.free()
    nested := Nested { before: 1 head: 2 first: 3 middle: 400 last: 5 tail: 6 after: 7 }
    base: ^Compact = ^nested
    assert base.middle == 400 && base.first == 3 && base.last == 5
    base.middle = 401
    assert nested.middle == 401 && nested.head == 2 && nested.tail == 6 && nested.after == 7
    cembedded := CEmbedded { head: 1 first: 2 middle: 300 last: 4 tail: 5 }
    cbase: ^Compact = ^cembedded
    assert cbase.middle == 300 && cbase.first == 2 && cbase.last == 4
    assert cbase.as(usize) - (^cembedded).as(usize) == 4
    tagged := UseTagged { tag: 1 before: 2 kind: Kind.Second payload: 3 after: 4 }
    assert tagged.kind == Kind.Second && tagged.payload == 3 && tagged.before == 2 && tagged.after == 4
    bits := Blocks { first: 1 low: 2 high: 3 last: 4 }
    assert bits.first == 1 && bits.low == 2 && bits.high == 3 && bits.last == 4
    bits.high = 10
    assert bits.low == 2 && bits.high == 10 && bits.first == 1 && bits.last == 4
    generic: Generic[u32] = { first: 10 middle: 20 last: 30 }
    assert generic.size == 8
    assert generic.first == 10 && generic.middle == 20 && generic.last == 30
    tuple := (1.as(u8), 2.as(u32), 3.as(u8))
    assert tuple.size == 12
    prn($"compact={Compact.size} c={COrder.size}")
}
¬
0
¬
compact=8 c=12

¬

¬
