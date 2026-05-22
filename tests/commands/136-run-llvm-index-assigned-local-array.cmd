use std.io

append_u64 :: fn (value: u64) {
    digits : [20]u8
    index := digits.count
    remaining := value

    on remaining == 0 => {
        index -= 1
        digits[index] = '0'
    } else {
        for remaining > 0 {
            index -= 1
            digits[index] = ('0'.as(u64) + (remaining % 10)).as(u8)
            remaining /= 10
        }
    }

    prn(digits[index..].as(string))
}

main :: fn () {
    append_u64(0)
    append_u64(12345)
}
¬
0
¬
0
12345

¬
delete
¬
--llvm
