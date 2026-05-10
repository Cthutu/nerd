main :: fn () -> i32 {
    nums: [..]i32

    nums.reserve(8)
    on nums.count != 0 => return 1
    on nums.capacity < 8 => return 2

    nums.push(10)
    nums.push(20)
    nums.clear()
    on nums.count != 0 => return 3
    on nums.capacity < 8 => return 4

    nums.push(30)
    on nums[0] != 30 => return 5

    nums.free()
    on nums != nil => return 6
    on nums.count != 0 => return 7
    on nums.capacity != 0 => return 8

    return 0
}
¬
0
¬

¬
delete
¬
--llvm-backend
