main :: fn () -> i32 {
    nums: [..]i32
    nums.push(1)
    nums.push(2)

    extra := [3, 4, 5]
    nums.append(extra[..])

    on nums.count != 5 => return 1
    on nums[0] != 1 => return 2
    on nums[1] != 2 => return 3
    on nums[2] != 3 => return 4
    on nums[3] != 4 => return 5
    on nums[4] != 5 => return 6

    return 0
}
¬
0
¬

¬
delete
¬
