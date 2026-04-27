use std.print

split :: fn (s: string, sep: string) -> [..]string {
    parts: [..]string

    on sep.count == 0 => {
        parts.push(s)
        return parts
    }

    start: usize = 0
    i: usize = 0
    for i + sep.count <= s.count {
        matched := yes
        for j: usize = 0; matched && j < sep.count; j += 1 {
            on s.data[i + j] != sep.data[j] => matched = no
        }

        on matched => {
            parts.push(s[start .. i])
            i += sep.count
            start = i
        } else {
            i += 1
        }
    }

    parts.push(s[start ..])
    return parts
}

main :: fn () -> i32 {
    empty: [..]string
    on empty.count != 0 => return 1
    on empty.capacity != 0 => return 2

    names: [4..]string = ["north", "south"]
    on names.count != 2 => return 3
    on names.capacity < 4 => return 4

    names.push("east")
    view := names[..]
    on view.count != 3 => return 5

    parts := split("look north", " ")
    on parts.count != 2 => return 6
    on parts.capacity < 2 => return 7

    prn($"{view[0]} {view[1]} {view[2]}")
    prn($"{parts[0]} {parts[1]}")
    return 0
}
¬
0
¬
north south east
look north

¬
¬
