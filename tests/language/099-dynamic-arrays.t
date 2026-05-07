use std.io

make_words :: fn () -> [..]string {
    words: [..]string
    words.push("look")
    words.push("north")
    return words
}

main :: fn () -> i32 {
    empty: [..]string
    on empty != nil => return 1
    on empty.count != 0 => return 2
    on empty.capacity != 0 => return 3

    names: [4..]string = ["north", "south"]
    on names.count != 2 => return 4
    on names.capacity < 4 => return 5

    names.push("east")
    on names.count != 3 => return 6

    extra :: ["west", "up"]
    names.append(extra[..])
    on names.count != 5 => return 7

    last := names.pop()
    on last != "up" => return 8
    on names.count != 4 => return 9
    names.push(last)

    view := names[..]
    on view.count != 5 => return 10

    prn($"{view[0]} {view[1]} {view[2]} {view[3]} {view[4]}")

    names.reserve(10)
    on names.capacity < 10 => return 11

    words := make_words()
    on words.count != 2 => return 12
    prn($"{words[0]} {words[1]}")
    words.free()

    nums: [..]i32
    nums.push(1)
    nums.push(2)
    nums.push(3)
    nums.push(4)
    nums.push(5)
    nums.delete(1)
    on nums.count != 4 => return 13
    on nums[0] != 1 => return 14
    on nums[1] != 3 => return 15
    on nums[2] != 4 => return 16
    on nums[3] != 5 => return 17
    nums.swap_delete(1)
    on nums.count != 3 => return 18
    on nums[0] != 1 => return 19
    on nums[1] != 5 => return 20
    on nums[2] != 4 => return 21
    nums.free()

    names.clear()
    on names.count != 0 => return 22
    names.free()
    on names != nil => return 23
    return 0
}
¬
0
¬
north south east west up
look north

¬
¬
