use std.io

main :: fn () -> i32 {
    text :: "hello"
    whole: string = text[..]
    ell: string = text[1..4]
    tail: string = text[2..]
    head: string = text[..2]

    prn($"whole = {whole}")
    prn($"ell = {ell}")
    prn($"tail = {tail}")
    prn($"head = {head}")
    prn($"count = {ell.count}")
    prn($"first byte = {ell.data[0]}")

    result :: on ell {
        "ell" => 7
        else => 1
    }

    return result + ell.data[1].as(i32)
}
¬
115
¬
whole = hello
ell = ell
tail = llo
head = he
count = 3
first byte = 101

¬
delete
¬
