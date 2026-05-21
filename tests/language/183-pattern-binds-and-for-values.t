use std.io

Maybe :: enum { None Some(i32) Pair(i32, i32) }

score :: fn (value: Maybe, wanted: i32) -> i32 {
    return on value {
        None => 0
        Some(for wanted) => 10
        Some(found) => found
        Pair(left, for wanted) => left + wanted
        Pair(left, right) => left + right
    }
}

main :: fn () -> i32 {
    some_wanted : Maybe = Some(7)
    some_other : Maybe = Some(4)
    pair_wanted : Maybe = Pair(2, 7)
    pair_other : Maybe = Pair(2, 3)

    a := score(some_wanted, 7)
    b := score(some_other, 7)
    c := score(pair_wanted, 7)
    d := score(pair_other, 7)

    prn($"pattern syntax {a} {b} {c} {d}")
    return a + b + c + d
}
¬
28
¬
pattern syntax 10 4 9 5

¬

¬
