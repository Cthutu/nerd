main :: fn () -> i32 {
    exits: [][](string, usize) = [
        [
            ("E", 1),
            ("W", 4),
        ],
        [("S", 2)],
    ]

    return (exits[0][1].1 + exits[1][0].1).as(i32)
}
¬
6
¬

¬
delete
¬
