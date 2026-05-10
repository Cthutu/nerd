main :: fn () -> i32 {
    empty: [..]i32
    seen := 0
    for item in empty {
        seen += item^
    }
    on seen != 0 => return 1

    nums: [..]i32
    nums.push(4)
    nums.push(5)
    nums.push(6)

    total := 0
    for item in nums {
        total += item^
    }

    on total != 15 => return 2
    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
