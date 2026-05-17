Point :: plex {
    x i32
    y i32
}

impl Order for Point {
    compare :: fn (lhs: Self, rhs: Self) -> i32 {
        lhs_score := lhs.x + lhs.y
        rhs_score := rhs.x + rhs.y
        return lhs_score - rhs_score
    }
}

main :: fn () -> i32 {
    low := Point { x: 1, y: 2 }
    same := Point { x: 2, y: 1 }
    high := Point { x: 3, y: 4 }

    prn($"lt={low < high}")
    prn($"le={low <= same}")
    prn($"gt={high > low}")
    prn($"ge={same >= low}")

    return on low < high && low <= same && high > low && same >= low {
        yes => 0
        else => 1
    }
}
¬
0
¬
lt=yes
le=yes
gt=yes
ge=yes

¬
hir 0
¬
