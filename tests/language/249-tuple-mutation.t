Holder :: plex { pair (i32, (i32, i32)) }
change :: fn (pair: ^(i32, i32)) { pair.1 += 1 }
main :: fn () -> i32 {
    pair := (20, 22)
    pair.0 = 21
    pair.1 += 1
    change(^pair)
    assert pair.1 == 24
    pair.1 = 22
    nested := ((1, 2), 3)
    nested.0.1 *= 4
    assert nested.0.1 == 8
    holder := Holder { pair: (1, (2, 3)) }
    holder.pair.1.0 += 2
    assert holder.pair.1.0 == 4
    return pair.0 + pair.1
}
¬
43
¬
¬
¬
