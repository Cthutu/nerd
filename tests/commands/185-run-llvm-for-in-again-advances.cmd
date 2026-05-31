main :: fn () -> i32 {
    nums : [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    defer nums.free()

    visits := 0
    total := 0
    for item in nums {
        visits += 1
        on item^ < 10 => again
        total += item^
    }

    on visits != 3 => return 1
    on total != 0 => return 2
    return 0
}
¬
0
¬

¬
delete
¬
