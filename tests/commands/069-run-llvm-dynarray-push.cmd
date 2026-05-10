main :: fn () -> i32 {
    nums: [..]i32
    on nums.count != 0 => return 1
    on nums.capacity != 0 => return 2

    nums.push(4)
    nums.push(5)

    on nums.count != 2 => return 3
    on nums.capacity < 2 => return 4
    on nums[0] != 4 => return 5
    on nums[1] != 5 => return 6

    view := nums[..]
    on view.count != 2 => return 7
    on view[1] != 5 => return 8

    return 0
}
¬
0
¬

¬
delete
¬
