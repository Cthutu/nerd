use std.io

main :: fn () -> i32 {
    text :: "hello"
    ell := text[1..4]
    prn($"text={text.bytes} ell={ell.bytes}")
    on text.bytes != text.count => return 1
    on ell.bytes != ell.count => return 2
    return (text.bytes + ell.bytes).as(i32)
}
¬
8
¬
text=5 ell=3

¬
delete
¬
