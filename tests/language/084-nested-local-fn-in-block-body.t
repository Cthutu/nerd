use std.io

lowercase_first :: fn (text: string) -> u8 {
    tolower :: fn (char: u8) -> u8 {
        return on char {
            'A' .. 'Z' => char + 32
            else => char
        }
    }

    chars := text
    for c in chars {
        return tolower(c^)
    }

    return 0
}

main :: fn () {
    prn($"{lowercase_first("ABC")}")
}
¬
0
¬
97

¬

¬
