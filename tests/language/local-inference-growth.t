-- Generic instantiation appends enough locals to relocate the table while
-- the caller's unannotated binding is still being inferred.
Grow :: plex [T] {
    value T
}

impl [T] Grow[T] {
    make :: fn (self: ^Self) -> Grow[T] {
        copy0 := self.value
        copy1 := self.value
        copy2 := self.value
        copy3 := self.value
        copy4 := self.value
        copy5 := self.value
        copy6 := self.value
        copy7 := self.value
        copy8 := self.value
        copy9 := self.value
        copy10 := self.value
        copy11 := self.value
        copy12 := self.value
        copy13 := self.value
        copy14 := self.value
        copy15 := self.value
        copy16 := self.value
        copy17 := self.value
        copy18 := self.value
        copy19 := self.value
        copy20 := self.value
        copy21 := self.value
        copy22 := self.value
        copy23 := self.value
        return self^
    }

    consume :: fn (self: ^Self, amount: T) -> T {
        return self.value + amount
    }
}

main :: fn () {
    source := Grow[i32] { value: 7 }
    target := source.make()
    amount := 5
    assert target.consume(amount) == 12
    other := Grow[u64] { value: 30 }
    second := other.make()
    extra := 4.as(u64)
    assert second.consume(extra) == 34
    prn($"local growth {target.consume(amount)} {second.consume(extra)}")
}
¬
0
¬
local growth 12 34

¬
¬
