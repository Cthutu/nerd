use std.io

sum_original :: fn (number: i32) -> i32 {
    on number == 0 => return 0
    rest := sum_original(number - 1)
    return number + rest
}

keep_text :: fn (text: string, recurse: bool) -> string {
    on recurse && text.count > 1 => {
        _ := keep_text(text[1..], no)
    }
    return text
}

with_default :: fn (number: i32, copied: i32 = number) -> i32 {
    on number <= 1 => return copied
    rest := with_default(number - 1)
    return number + copied + rest
}

with_offset :: fn (number: i32, copied: i32 = number + 10) -> i32 {
    on number <= 1 => return copied
    rest := with_offset(number - 1)
    return number + copied + rest
}

main :: fn () {
    prn($"{sum_original(4)}")
    prn(keep_text("alpha", no))
    prn(keep_text("bravo", yes))
    prn($"{with_default(3)}")
    prn($"{with_offset(3)}")
}
¬
0
¬
10
alpha
bravo
11
41

¬

¬
