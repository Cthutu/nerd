main :: fn () -> i32 {
    nums: [..]i32
    nums.push(4)
    nums.resize(3)

    on nums.count != 3 => return 1
    on nums.capacity < 3 => return 2
    on nums[0] != 4 => return 3
    on nums[1] != 0 => return 4
    on nums[2] != 0 => return 5

    nums.resize_undefined(1)
    on nums.count != 1 => return 6
    on nums[0] != 4 => return 7

    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
