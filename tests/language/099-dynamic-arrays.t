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

    view := names[..]
    on view.count != 5 => return 8

    prn($"{view[0]} {view[1]} {view[2]} {view[3]} {view[4]}")

    names.reserve(10)
    on names.capacity < 10 => return 9

    words := make_words()
    on words.count != 2 => return 10
    prn($"{words[0]} {words[1]}")
    words.free()

    names.clear()
    on names.count != 0 => return 11
    names.free()
    on names != nil => return 12
    return 0
}
¬
0
¬
north south east west up
look north

¬
¬
