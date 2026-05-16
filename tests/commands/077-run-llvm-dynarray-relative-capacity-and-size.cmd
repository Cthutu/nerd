main :: fn () -> i32 {
    nums: [..]i32

    nums.push(4)
    nums.reserve_extra(4)
    on nums.count != 1 => return 1
    on nums.capacity < 5 => return 2

    nums.extend(2)
    on nums.count != 3 => return 3
    on nums[0] != 4 => return 4
    on nums[1] != 0 => return 5
    on nums[2] != 0 => return 6

    nums.extend_undefined(2)
    on nums.count != 5 => return 7
    nums[3] = 8
    nums[4] = 9
    on nums[3] != 8 => return 8
    on nums[4] != 9 => return 9

    nums.resize_to(2)
    on nums.count != 2 => return 10

    nums.resize_undefined_to(1)
    on nums.count != 1 => return 11
    on nums[0] != 4 => return 12

    nums.free()
    return 0
}
¬
0
¬

¬
delete
¬
