use std.io

Pair :: plex {
    x i32
    y i32
}

score :: fn (size: u32) -> u32 {
    return on size {
        0..3 as matched on matched == 2 => matched + 20
        0..3 => 10
        10 as size on size == 10 => size * 10
        else as size => size + 100
    }
}

pair_score :: fn (pair: Pair) -> i32 {
    return on pair {
        { x: 0, y: as y } => y + 100
        { x: as x, y: 1 } => x
        else => 0
    }
}

main :: fn () {
    prn($"1: {score(1)}")
    prn($"2: {score(2)}")
    prn($"10: {score(10)}")
    prn($"50: {score(50)}")
    prn($"pair: {pair_score(Pair { x: 0, y: 7 })}")
}
¬
0
¬
1: 10
2: 22
10: 100
50: 150
pair: 107

¬
delete
¬
