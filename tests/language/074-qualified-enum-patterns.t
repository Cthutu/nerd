Maybe :: enum { None Some(i32) Pair(i32, i32) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        Maybe.None => 0
        Maybe.Some(as x) => x
        Maybe.Pair(as left, as right) => left + right
    }
}

main :: fn () -> i32 {
    return score(Maybe.Pair(10, 20))
}
¬
30
¬
¬¬