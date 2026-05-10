main :: fn () -> i32 {
    nums: [4..]i32 = [1, 2]

    on nums.count != 2 => return 1
    on nums.capacity < 4 => return 2
    on nums[0] != 1 => return 3
    on nums[1] != 2 => return 4

    return 0
}
¬
0
¬

¬
delete
¬
