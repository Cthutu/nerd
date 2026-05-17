Point :: plex {
    x i32
    y i32
}

impl Eq for Point {
    eq :: fn (lhs: Self, rhs: Self) -> bool {
        return lhs.x == rhs.x && lhs.y == rhs.y
    }
}

main :: fn () -> i32 {
    a := Point { x: 3, y: 4 }
    b := Point { x: 3, y: 4 }
    c := Point { x: 3, y: 5 }
    prn($"same={a == b}")
    prn($"diff={a != c}")
    return on a == b && a != c {
        yes => 0
        else => 1
    }
}
¬
0
¬
same=yes
diff=yes

¬
hir 0
¬
