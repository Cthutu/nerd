use std.io

Maybe :: enum { None Some(i32) Pair(i32, i32) Text(string) }

score :: fn (value: Maybe) -> i32 {
    return on value {
        None => 0
        Some(as x) => x
        Pair(as left, as right) => left + right
        Text(_) => 100
    }
}

main :: fn () -> i32 {
    a : Maybe = None
    b : Maybe = Some(5)
    c := Maybe.Pair(10, 20)
    d : Maybe = Text("hello")

    prn($"scores {score(a)} {score(b)} {score(c)} {score(d)}")

    return score(c)
}
¬
30
¬
scores 0 5 30 100

¬
delete
¬
--llvm-backend
