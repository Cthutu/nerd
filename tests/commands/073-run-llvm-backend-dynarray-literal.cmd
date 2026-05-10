main :: fn () -> i32 {
    nums: [..]i32 = [7, 8, 9]

    on nums.count != 3 => return 1
    on nums.capacity < 3 => return 2
    on nums[0] != 7 => return 3
    on nums[1] != 8 => return 4
    on nums[2] != 9 => return 5

    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
