main :: fn () -> i32 {
    nums: [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    nums.push(4)

    last := nums.pop()
    on last != 4 => return 1
    on nums.count != 3 => return 2

    nums.delete(1)
    on nums.count != 2 => return 3
    on nums[0] != 1 => return 4
    on nums[1] != 3 => return 5

    nums.push(5)
    nums.swap_delete(1)
    on nums.count != 2 => return 6
    on nums[0] != 1 => return 7
    on nums[1] != 5 => return 8

    return 0
}
¬
0
¬

¬
delete
¬
